/*
 * Copyright 2019 ZWX
 * All rights reserved.
 *
 */

#include <rtthread.h>
#ifdef BSP_USING_SD

#include "drv_sd.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_sd.h"
#include "fsl_sdmmc_host.h"
#include "fsl_iomuxc.h"

//#define DRV_DEBUG
#define LOG_TAG             "drv.sd"
#include <drv_log.h>

/*! @brief Card semaphore  */
static rt_sem_t s_CardAccessSemaphore = NULL;
static rt_sem_t s_CardDetectSemaphore = NULL;

/*! @brief Card descriptor. */
static sd_card_t g_sd;

/*! @brief SD card detect flag  */
static bool s_cardInserted = false;

/*! @brief Data block count accessed in card */
#define DATA_BLOCK_COUNT (5U)
/*! @brief Start data block number accessed in card */
#define DATA_BLOCK_START (2U)
/*! @brief Data buffer size. */
#define DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * DATA_BLOCK_COUNT)

/* @brief decription about the read/write buffer
* The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
* block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
* can define the block length by yourself if the card supports partial access.
* The address of the read/write buffer should align to the specific DMA data buffer address align value if
* DMA transfer is used, otherwise the buffer address is not important.
* At the same time buffer address/size should be aligned to the cache line size if cache is supported.
*/
/*! @brief Data written to the card */
SDK_ALIGN(uint8_t g_dataWrite[SDK_SIZEALIGN(DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_dataRead[SDK_SIZEALIGN(DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

/*!
* @brief call back function for SD card detect.
*
* @param isInserted  true,  indicate the card is insert.
*                    false, indicate the card is remove.
* @param userData
*/
static void SDCARD_DetectCallBack(bool isInserted, void *userData);

/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
    .cardInserted = SDCARD_DetectCallBack,
    .cardRemoved = SDCARD_DetectCallBack,
};

static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    s_cardInserted = isInserted;
    rt_sem_release(s_CardDetectSemaphore);
}

static void CardDetectTask(void *pvParameters)
{
    while (true)
    {
        if (!s_cardInserted)
        {
            /* power off card */
            SD_PowerOffCard(g_sd.sdhost.base, g_sd.usrParam.pwr);
        }

        /* take card detect semaphore */
        rt_sem_take(s_CardDetectSemaphore, RT_WAITING_FOREVER);
        if (s_cardInserted)
        {
            LOG_I("Card inserted.");
            /* power on the card */
            SD_PowerOnCard(g_sd.sdhost.base, g_sd.usrParam.pwr);
            /* Init card. */
            if (SD_CardInit(&g_sd))
            {
                LOG_E("SD card init failed.");
                return;
            }
            rt_sem_release(s_CardAccessSemaphore);
        }
        else
        {
            LOG_I("Please insert a card into board.");
        }
    }
}

static status_t SDCARD_ReadWrite(sd_card_t *card, bool isReadOnly)
{
    if (isReadOnly)
    {
        rt_kprintf("\r\nRead one data block......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            rt_kprintf("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        rt_kprintf("Read multiple data blocks......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            rt_kprintf("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }
    else
    {
        memset(g_dataWrite, 0x67U, sizeof(g_dataWrite));

        rt_kprintf("\r\nWrite/read one data block......\r\n");
		
		if (kStatus_Success != SD_WriteBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            rt_kprintf("Write one data block failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            rt_kprintf("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        rt_kprintf("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            rt_kprintf("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        rt_kprintf("The read/write content is consistent.\r\n");

        rt_kprintf("Write/read multiple data blocks......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            rt_kprintf("Write multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));

        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            rt_kprintf("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        rt_kprintf("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            rt_kprintf("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        rt_kprintf("The read/write content is consistent.\r\n");

        rt_kprintf("Erase multiple data blocks......\r\n");
        if (kStatus_Success != SD_EraseBlocks(card, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            rt_kprintf("Erase multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }

    return kStatus_Success;
}

static void AccessCardTask(void *pvParameters)
{
    sd_card_t *card = &g_sd;
    bool isReadOnly;

    /* take card access semaphore */
    rt_sem_take(s_CardAccessSemaphore, RT_WAITING_FOREVER);

	if (SD_IsCardPresent(card) == false)
	{
		/* take card access semaphore */
		rt_sem_take(s_CardAccessSemaphore, RT_WAITING_FOREVER);
	}

	rt_kprintf("Read/Write/Erase the card continuously until encounter error......");
	/* Check if card is readonly. */
	isReadOnly = SD_CheckReadOnly(card);

	SDCARD_ReadWrite(card, isReadOnly);
}

int rt_hw_sd_Init(void)
{
    int result = RT_EOK;
	rt_thread_t tid = RT_NULL;	

    /*configure system pll PFD2 fractional divider to 0x18*/
    CLOCK_InitSysPfd(kCLOCK_Pfd2, 0x18U);
    /* Configure USDHC clock source and divider */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 0U);
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 1U);

    s_CardAccessSemaphore = rt_sem_create("SDCardAccessSem", 0, RT_IPC_FLAG_FIFO);
    s_CardDetectSemaphore = rt_sem_create("SDCardDetectSem", 0, RT_IPC_FLAG_FIFO);
	if ((s_CardAccessSemaphore == RT_NULL) || (s_CardAccessSemaphore == RT_NULL))
	{
		LOG_E("create dynamic semaphore failed.\n");
		return -1;
	}
	
    g_sd.sdhost.base = SD_HOST_BASEADDR;
    g_sd.sdhost.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd = &s_sdCardDetect;

	NVIC_SetPriority(SD_HOST_IRQ,  5U);
	
    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        LOG_E("SD host init fail.\n");
        return -1;
    }
	else
	{
		LOG_D("SD host init success.\n");
	}
	
    tid = rt_thread_create("app_CardDetect",   /* thread name */
                            CardDetectTask,    /* thread entry */
	                        (void*)0,   /* thread parameter */
							512,        /* thread stack size : byte */
                            RT_THREAD_PRIORITY_MAX - 3, /* thread priority : 0 is the max high priority */
							5); /* thread tick : when thread priority is same */	
    if (tid != RT_NULL)
        rt_thread_startup(tid);							
	
	return result;
}
//INIT_APP_EXPORT(rt_hw_sd_Init);



void BOARD_SD_Pin_Config(uint32_t speed, uint32_t strength)
{
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_00_USDHC1_CMD,
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |
                            IOMUXC_SW_PAD_CTL_PAD_DSE(strength));
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_01_USDHC1_CLK,
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(0) |
                            IOMUXC_SW_PAD_CTL_PAD_DSE(strength));
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_02_USDHC1_DATA0,
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |
                            IOMUXC_SW_PAD_CTL_PAD_DSE(strength));
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_03_USDHC1_DATA1,
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |
                            IOMUXC_SW_PAD_CTL_PAD_DSE(strength));
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_04_USDHC1_DATA2,
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |
                            IOMUXC_SW_PAD_CTL_PAD_DSE(strength));
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_05_USDHC1_DATA3,
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |
                            IOMUXC_SW_PAD_CTL_PAD_DSE(strength));
}

void BOARD_MMC_Pin_Config(uint32_t speed, uint32_t strength)
{
}

#define DRV_DEBUG
#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH

void sd_sample(void)
{
	AccessCardTask(0);
}
//MSH_CMD_EXPORT(sd_sample, sd_sample);


#endif  /* FINSH_USING_MSH */
#endif  /* DRV_DEBUG */
#endif  /* BSP_USING_SD */



