/*
 * Copyright 2019 ZWX
 * All rights reserved.
 * Change Logs:
 * Date           Author       Notes
 * 2019-07-19     zwx          first implementatio
 */
#include <rtthread.h>
#include "board.h"
#include <rtdevice.h>

#ifdef RT_USING_FINSH
    #include <finsh.h>
#endif

#include "drv_eth.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_enet.h"
#include "fsl_common.h"
#include "fsl_phy.h"
#include "ethernetif.h"

#define DRV_DEBUG
#define LOG_TAG             "drv.eth"
#include <drv_log.h> 

#define MAX_ADDR_LEN        6
#define PHY_ADDRESS         0x02u  /* Phy address of enet port 0. */

#define ENET_RXBD_NUM      (5)
#define ENET_TXBD_NUM      (5)
#define ENET_RXBUFF_SIZE   (ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE   (ENET_FRAME_MAX_FRAMELEN)

#define ENET_RING_NUM       1U

AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);

ALIGN(ENET_BUFF_ALIGNMENT) rt_uint8_t g_txDataBuff[ENET_TXBD_NUM][RT_ALIGN(ENET_TXBUFF_SIZE, ENET_BUFF_ALIGNMENT)];
ALIGN(ENET_BUFF_ALIGNMENT) rt_uint8_t g_rxDataBuff[ENET_RXBD_NUM][RT_ALIGN(ENET_RXBUFF_SIZE, ENET_BUFF_ALIGNMENT)];

/*  Defines Ethernet Autonegotiation Timeout during initialization. 
 *  Set it to 0 to disable the waiting. */ 
#ifndef ENET_ATONEGOTIATION_TIMEOUT
    #define ENET_ATONEGOTIATION_TIMEOUT     (0xFFFU)
#endif

struct rt_imxrt_eth
{
    /* inherit from ethernet device */
    struct eth_device parent;

    enet_handle_t enet_handle;
    ENET_Type *enet_base;
    enet_data_error_stats_t error_statistic;
    rt_uint8_t  dev_addr[MAX_ADDR_LEN];         /* hw address   */

    rt_bool_t tx_is_waiting;
    struct rt_semaphore tx_wait;

    enet_mii_speed_t speed;
    enet_mii_duplex_t duplex;
};


static struct rt_imxrt_eth imxrt_eth_device;
uint8_t g_drv_eth_en = 0;

static void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 1000000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

static void enet_io_init(void)
{
	gpio_pin_config_t gpio_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	
    GPIO_PinInit(GPIO1, 9, &gpio_config);
    GPIO_PinInit(GPIO1, 10, &gpio_config);

	IOMUXC_SetPinMux(
		IOMUXC_GPIO_B1_10_ENET_REF_CLK,         /* GPIO_B1_10 is configured as ENET_REF_CLK */
		1U);  	                                /* zwx set REF_CLK as output pin */
	IOMUXC_SetPinConfig(
		IOMUXC_GPIO_B1_10_ENET_REF_CLK,         /* GPIO_B1_10 PAD functional properties : */
		0x31U);                                 /* Slew Rate Field: Fast Slew Rate
												 Drive Strength Field: R0/6
												 Speed Field: low(50MHz)
												 Open Drain Enable Field: Open Drain Disabled
												 Pull / Keep Enable Field: Pull/Keeper Disabled
												 Pull / Keep Select Field: Keeper
												 Pull Up / Down Config. Field: 100K Ohm Pull Down
												 Hyst. Enable Field: Hysteresis Disabled */	
}

static void enet_clk_init(void)
{
    const clock_enet_pll_config_t config = 
    {
		.enableClkOutput = true,
		.enableClkOutput25M = false,
	     .loopDivider = 1
	};
    CLOCK_InitEnetPll(&config);
	
	IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1TxClkOutputDir, true);
}

static void enet_phy_reset_by_gpio(void)
{
    /* pull up the ENET_INT before RESET. */
    GPIO_WritePinOutput(GPIO1, 10, 1);
    GPIO_WritePinOutput(GPIO1, 9, 0);
    delay();
    GPIO_WritePinOutput(GPIO1, 9, 1);	
}

static void enet_callback(ENET_Type *base, enet_handle_t *handle, enet_event_t event, void *param)
{
	rt_err_t result;
	struct rt_imxrt_eth *eth = (struct rt_imxrt_eth *)param;
	
    switch (event)
    {
        case kENET_RxEvent:
			ENET_DisableInterrupts(eth->enet_base, kENET_RxFrameInterrupt);
			
			result = eth_device_ready(&(eth->parent));
			if (result != RT_EOK)
			{
				rt_kprintf("RX err =%d\n", result);
			}				
            break;
        case kENET_TxEvent:		
			if (eth->tx_is_waiting == RT_TRUE)
			{
				eth->tx_is_waiting = RT_FALSE;
				rt_sem_release(&eth->tx_wait);
			}
			break;
        default:
            break;
    }	
}


static void ethernetif_phy_init(ENET_Type *base, uint32_t phyAddr, uint32_t srcClock_Hz)
{
    uint32_t sysClock;
    status_t status;
    bool link = false;
    uint32_t count = 0;
    phy_speed_t speed;
    phy_duplex_t duplex;

    LOG_D(("Initializing PHY...\n"));

    while ((count < ENET_ATONEGOTIATION_TIMEOUT) && (!link))
    {
        status = PHY_Init(base, phyAddr, srcClock_Hz);

        if (kStatus_Success == status)
        {
            PHY_GetLinkStatus(base, phyAddr, &link);
			LOG_D("PHY Auto-negotiation ok.\n");
        }
        else if (kStatus_PHY_AutoNegotiateFail == status)
        {
            LOG_E(("PHY Auto-negotiation failed. Please check the ENET cable connection and link partner setting.\n"));
        }
        else
        {
            LOG_E("\r\nCannot initialize PHY.\r\n", 0);
        }

        count++;
    }

    if (link)
    {
        /* Get the actual PHY link speed. */
        PHY_GetLinkSpeedDuplex(base, phyAddr, &speed, &duplex);
        /* Change the MII speed and duplex for actual link status. */
        imxrt_eth_device.speed = (enet_mii_speed_t)speed;
        imxrt_eth_device.duplex = (enet_mii_duplex_t)duplex;
		if (kPHY_Speed10M == speed)
		{
			LOG_D("eth speed : 10M\n");
		}
		else
		{
			LOG_D("eth speed : 100M\n");
		}         

		if (kPHY_HalfDuplex == duplex)
		{
			LOG_D("eth duplex : half dumplex\n");
		}
		else
		{
			LOG_D("eth duplex : full dumplex\n");
		}				
    }
}

/* initialize enet */
static void enet_config(void)
{
    enet_config_t config;
    uint32_t sysClock;
    uint32_t instance;
    static ENET_Type *const enetBases[] = ENET_BASE_PTRS;
    static const IRQn_Type enetTxIrqId[] = ENET_Transmit_IRQS;
    /*! @brief Pointers to enet receive IRQ number for each instance. */
    static const IRQn_Type enetRxIrqId[] = ENET_Receive_IRQS;	

    /* prepare the buffer configuration. */
    enet_buffer_config_t buffConfig =
    {
        ENET_RXBD_NUM,
        ENET_TXBD_NUM,
        SDK_SIZEALIGN(ENET_RXBUFF_SIZE, ENET_BUFF_ALIGNMENT),
        SDK_SIZEALIGN(ENET_TXBUFF_SIZE, ENET_BUFF_ALIGNMENT),
        &g_rxBuffDescrip[0],
        &g_txBuffDescrip[0],
        &g_rxDataBuff[0][0],
        &g_txDataBuff[0][0],
    };	
	
	sysClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);
	
    ENET_GetDefaultConfig(&config);
    config.ringNum = ENET_RING_NUM;
    config.miiSpeed = imxrt_eth_device.speed;
    config.miiDuplex = imxrt_eth_device.duplex;

#ifdef RT_LWIP_USING_HW_CHECKSUM
	config.rxAccelerConfig = ENET_RACC_PRODIS_MASK | ENET_RACC_IPDIS_MASK;
	config.txAccelerConfig = ENET_TACC_IPCHK_MASK | ENET_TACC_PROCHK_MASK;
#endif

	config.interrupt |= kENET_RxFrameInterrupt | kENET_TxFrameInterrupt | kENET_TxBufferInterrupt;
	
	for (instance = 0; instance < ARRAY_SIZE(enetBases); instance++)
	{
		if (enetBases[instance] == imxrt_eth_device.enet_base)
		{
            NVIC_SetPriority(enetRxIrqId[instance], ENET_PRIORITY);
            NVIC_SetPriority(enetTxIrqId[instance], ENET_PRIORITY);			
		}
	}

    LOG_D("init\n");
    ENET_Init(imxrt_eth_device.enet_base, &imxrt_eth_device.enet_handle, &config, &buffConfig, &imxrt_eth_device.dev_addr[0], sysClock);
    LOG_D("set call back\n");
    ENET_SetCallback(&imxrt_eth_device.enet_handle, enet_callback, &imxrt_eth_device);
    LOG_D("active read\n");
    ENET_ActiveRead(imxrt_eth_device.enet_base);
}

/* initialize the interface */
static rt_err_t rt_imxrt_eth_init(rt_device_t dev)
{
    LOG_D("rt_imxrt_eth_init...\n");
    enet_config();

    return RT_EOK;
}

static rt_err_t rt_imxrt_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    LOG_D("rt_imxrt_eth_open...\n");
    return RT_EOK;
}

static rt_err_t rt_imxrt_eth_close(rt_device_t dev)
{
    LOG_D("rt_imxrt_eth_close...\n");
    return RT_EOK;
}

static rt_size_t rt_imxrt_eth_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    LOG_D("rt_imxrt_eth_read...\n");
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_size_t rt_imxrt_eth_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    LOG_D("rt_imxrt_eth_write...\n");
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t rt_imxrt_eth_control(rt_device_t dev, int cmd, void *args)
{
    LOG_D("rt_imxrt_eth_control...\n");
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args) 
        {
			rt_memcpy(args, imxrt_eth_device.dev_addr, 6);
		}
		else
		{
			return -RT_ERROR;
		}		 
        break;

    default :
        break;
    }

    return RT_EOK;
}


/* transmit packet. */
static rt_err_t rt_imxrt_eth_tx(rt_device_t dev, struct pbuf *p)
{
	rt_err_t result = RT_EOK;
	enet_handle_t * enet_handle = &imxrt_eth_device.enet_handle;

	RT_ASSERT(p != NULL);
	RT_ASSERT(enet_handle != RT_NULL);

//	LOG_D("rt_imxrt_eth_tx: %d\n", p->len);
	
#ifdef ETH_TX_DUMP
	packet_dump("send", p);
#endif
	
	do
	{
		result = ENET_SendFrame(imxrt_eth_device.enet_base, enet_handle, (const uint8_t *)(p->payload), p->tot_len);

		if (result == kStatus_ENET_TxFrameBusy)
		{
			imxrt_eth_device.tx_is_waiting = RT_TRUE;
			rt_sem_take(&imxrt_eth_device.tx_wait, RT_WAITING_FOREVER);
		}

	}
	while (result == kStatus_ENET_TxFrameBusy);

	return RT_EOK;

}

/* reception packet. */
struct pbuf *rt_imxrt_eth_rx(rt_device_t dev)
{
	status_t status;
	uint32_t length = 0;
	enet_handle_t *enet_handle = &imxrt_eth_device.enet_handle;
	ENET_Type *enet_base = imxrt_eth_device.enet_base;
	struct pbuf *p = NULL;
	enet_data_error_stats_t *error_statistic = &imxrt_eth_device.error_statistic;

    /* Get the Frame size */
    status = ENET_GetRxFrameSize(enet_handle, &length);
	
	if (kStatus_ENET_RxFrameEmpty != status)
	{
        /* Call enet_read_frame when there is a received frame. */
        if (length != 0)
        {
			/* We allocate a pbuf chain of pbufs from the pool. */
			p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
			if (p != NULL)
			{
                status = ENET_ReadFrame(enet_base, enet_handle, p->payload, length);
				if (status == kStatus_Success)
				{
#ifdef ETH_RX_DUMP
					packet_dump("recv", p);
#endif
					return p;
				}
				else
				{
					LOG_D(" A frame read failed");
					pbuf_free(p);
				}
			}
			else
			{
				LOG_D(" pbuf_alloc faild");
			}
		}
		else
		{
            /* Update the received buffer when error happened. */
            if (status == kStatus_ENET_RxFrameError)
            {
				LOG_W("ENET_GetRxFrameSize: kStatus_ENET_RxFrameError");
				/* Update the received buffer when error happened. */
				/* Get the error information of the received g_frame. */
				ENET_GetRxErrBeforeReadFrame(enet_handle, error_statistic);
				/* update the receive buffer. */
				ENET_ReadFrame(enet_base, enet_handle, NULL, 0);
			}
		}
	}
    ENET_EnableInterrupts(enet_base, kENET_RxFrameInterrupt);
    
	return p;
}


/* phy monitor */
static void phy_monitor_thread_entry(void *parameter)
{
    status_t status;
    bool link = false;
	bool new_link = false;
    phy_speed_t speed;
    phy_duplex_t duplex;	

	rt_PHY_Init(imxrt_eth_device.enet_base, PHY_ADDRESS, CLOCK_GetFreq(kCLOCK_CoreSysClk));

	while(1)
	{	
		status = PHY_GetLinkStatus(imxrt_eth_device.enet_base, PHY_ADDRESS, &new_link);
		if ((status == kStatus_Success) && (link != new_link))
		{
			link = new_link;
			if (link)   // link up
			{
                PHY_GetLinkSpeedDuplex(imxrt_eth_device.enet_base, PHY_ADDRESS, &speed, &duplex);	
                if (kPHY_Speed10M == speed)
                {
                    LOG_D("eth speed : 10M\n");
                }
                else
                {
                    LOG_D("eth speed : 100M\n");
                }         

                if (kPHY_HalfDuplex == duplex)
                {
                    LOG_D("eth duplex : half dumplex\n");
                }
                else
                {
                    LOG_D("eth duplex : full dumplex\n");
                }		

                if (   (imxrt_eth_device.speed  != (enet_mii_speed_t)speed)
                    || (imxrt_eth_device.duplex != (enet_mii_duplex_t)duplex))
                {
                    imxrt_eth_device.speed  = (enet_mii_speed_t)speed;
                    imxrt_eth_device.duplex = (enet_mii_duplex_t)duplex;

                    LOG_D("link up, and update eth mode.\n");
                    rt_imxrt_eth_init((rt_device_t)&imxrt_eth_device);
                }
                else
                {
                    LOG_D("link up, eth not need re-config.\n");
                }			
                eth_device_linkchange(&imxrt_eth_device.parent, RT_TRUE);				
			}
			else
			{
                LOG_D("link down.\n");
                eth_device_linkchange(&imxrt_eth_device.parent, RT_FALSE);				
			}			
		}
		rt_thread_delay(RT_TICK_PER_SECOND * 2);
	}
}


static int rt_hw_imxrt_eth_init(void)
{
	rt_err_t state;
	
	enet_io_init();
	enet_clk_init();
	enet_phy_reset_by_gpio();

    imxrt_eth_device.dev_addr[0] = 0x02;
    imxrt_eth_device.dev_addr[1] = 0x12;
    imxrt_eth_device.dev_addr[2] = 0x13;	
    imxrt_eth_device.dev_addr[3] = 0x66;
    imxrt_eth_device.dev_addr[4] = 0x88;
    imxrt_eth_device.dev_addr[5] = 0x68;
	
    imxrt_eth_device.speed  = kENET_MiiSpeed100M;
    imxrt_eth_device.duplex = kENET_MiiFullDuplex;

	imxrt_eth_device.enet_base = ENET;

    imxrt_eth_device.parent.parent.init       = rt_imxrt_eth_init;
    imxrt_eth_device.parent.parent.open       = rt_imxrt_eth_open;
    imxrt_eth_device.parent.parent.close      = rt_imxrt_eth_close;
    imxrt_eth_device.parent.parent.read       = rt_imxrt_eth_read;
    imxrt_eth_device.parent.parent.write      = rt_imxrt_eth_write;
    imxrt_eth_device.parent.parent.control    = rt_imxrt_eth_control;
    imxrt_eth_device.parent.parent.user_data  = RT_NULL;

    imxrt_eth_device.parent.eth_rx     = rt_imxrt_eth_rx;
    imxrt_eth_device.parent.eth_tx     = rt_imxrt_eth_tx;

    LOG_D("sem init: tx_wait\r\n");
    /* init tx semaphore */
    rt_sem_init(&imxrt_eth_device.tx_wait, "tx_wait", 0, RT_IPC_FLAG_FIFO);

    /* register eth device */
    LOG_D("eth_device_init start\r\n");
    state = eth_device_init(&(imxrt_eth_device.parent), "e0");
    if (RT_EOK == state)
    {
        LOG_D("eth_device_init success\r\n");
    }
    else
    {
        LOG_D("eth_device_init faild: %d\r\n", state);
    }

	g_drv_eth_en = 1; /* eth rst pin is conflict with led pin */
	
//	eth_device_linkchange(&imxrt_eth_device.parent, RT_FALSE);

    /* start phy monitor */
    {
        rt_thread_t tid;
        tid = rt_thread_create("phy",
                               phy_monitor_thread_entry,
                               RT_NULL,
                               512,
                               RT_THREAD_PRIORITY_MAX - 2,
                               2);
        if (tid != RT_NULL)
            rt_thread_startup(tid);
    }

	return state;
}


INIT_DEVICE_EXPORT(rt_hw_imxrt_eth_init);

#ifdef FINSH_USING_MSH

void phy_read(uint32_t phyReg)
{
    uint32_t data;
    status_t status;

    status = PHY_Read(imxrt_eth_device.enet_base, PHY_ADDRESS, phyReg, &data);
    if (kStatus_Success == status)
    {
        rt_kprintf("PHY_Read: %02X --> %08X", phyReg, data);
    }
    else
    {
        rt_kprintf("PHY_Read: %02X --> faild", phyReg);
    }
}

void phy_write(uint32_t phyReg, uint32_t data)
{
    status_t status;

    status = PHY_Write(imxrt_eth_device.enet_base, PHY_ADDRESS, phyReg, data);
    if (kStatus_Success == status)
    {
        rt_kprintf("PHY_Write: %02X --> %08X\n", phyReg, data);
    }
    else
    {
        rt_kprintf("PHY_Write: %02X --> faild\n", phyReg);
    }
}

void phy_state(void)
{
    phy_speed_t speed;
    phy_duplex_t duplex;	
	
	PHY_GetLinkSpeedDuplex(imxrt_eth_device.enet_base, PHY_ADDRESS, &speed, &duplex);	
	if (kPHY_Speed10M == speed)
	{
		rt_kprintf("[PHY] eth speed : 10M\n");
	}
	else
	{
		rt_kprintf("[PHY] eth speed : 100M\n");
	}         

	if (kPHY_HalfDuplex == duplex)
	{
		rt_kprintf("[PHY] eth duplex : half dumplex\n");
	}
	else
	{
		rt_kprintf("[PHY] eth duplex : full dumplex\n");
	}		
}

MSH_CMD_EXPORT(phy_read, phy_read);
MSH_CMD_EXPORT(phy_write, phy_write);
MSH_CMD_EXPORT(phy_state, phy_state);
#endif




