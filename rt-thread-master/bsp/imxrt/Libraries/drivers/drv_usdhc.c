/*
 * Copyright 2019 ZWX
 * All rights reserved.
 *
 */

#include <rtthread.h>

#include "drv_usdhc.h"
#include "fsl_clock.h"
#include "fsl_usdhc.h"
#include "fsl_sd.h"
#include "fsl_sdmmc_spec.h"

//#define DRV_DEBUG
#define LOG_TAG             "drv.usdhc"
#include <drv_log.h>

#define SD_FREQ_MIN  SDMMC_CLOCK_400KHZ
#define SD_FREQ_MAX  SD_CLOCK_25MHZ

/*! @brief Card descriptor. */
static struct rt_mmcsd_host *host;
static rt_sd_card_t *sd_card; 

/*! @brief SD card detect flag  */
static bool s_cardInserted = false;

extern uint32_t g_sdmmc[SDK_SIZEALIGN(SDMMC_GLOBAL_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)];

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

/* SD detect interrupt callback function */
static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
	s_cardInserted = isInserted;
	if(s_cardInserted)
	{
		LOG_D("Card inserted\n");
		host->card = RT_NULL;
		mmcsd_change(host);		
	}
	else
	{
		LOG_D("Card removed\n");
		mmcsd_change(host);
	}
}

/* 32 byte alignment is best */
static void rt_ushdc_request(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
	SDMMCHOST_DATA data = {0U};
	struct rt_mmcsd_cmd *cmd;
	rt_sd_card_t *card;
	rt_uint32_t *buf = NULL;
	
	card = (rt_sd_card_t *)host->private_data;	
	cmd = req->cmd;
	
	LOG_D("cmd->cmd_code: %02d, cmd->arg: %08x --> ", cmd->cmd_code, cmd->arg);

    command.index = cmd->cmd_code;
    command.argument = cmd->arg;
    if (cmd->cmd_code == STOP_TRANSMISSION)
        command.type = kCARD_CommandTypeAbort;
    else
        command.type = kCARD_CommandTypeNormal;	
    switch (cmd->flags & RESP_MASK)
    {
    case RESP_NONE:
        command.responseType = kCARD_ResponseTypeNone;
        break;
    case RESP_R1:
        command.responseType = kCARD_ResponseTypeR1;
        break;
    case RESP_R1B:
        command.responseType = kCARD_ResponseTypeR1b;
        break;
    case RESP_R2:
        command.responseType = kCARD_ResponseTypeR2;
        break;
    case RESP_R3:
        command.responseType = kCARD_ResponseTypeR3;
        break;
    case RESP_R4:
        command.responseType = kCARD_ResponseTypeR4;
        break;
    case RESP_R6:
        command.responseType = kCARD_ResponseTypeR6;
        break;
    case RESP_R7:
        command.responseType = kCARD_ResponseTypeR7;
        break;
    case RESP_R5:
        command.responseType = kCARD_ResponseTypeR5;
        break;
        /*
        case RESP_R5B:
            fsl_command.responseType = kCARD_ResponseTypeR5b;
            break;
        */
    default:
        RT_ASSERT(NULL);
    }
	if ((cmd->cmd_code == WRITE_BLOCK) || (cmd->cmd_code == WRITE_MULTIPLE_BLOCK) || 
		(cmd->cmd_code == READ_SINGLE_BLOCK) || (cmd->cmd_code == READ_MULTIPLE_BLOCK) ||
	    (cmd->cmd_code == STOP_TRANSMISSION) || (cmd->cmd_code == ERASE_GROUP_START) ||
	    (cmd->cmd_code == ERASE_GROUP_END) || (cmd->cmd_code == ERASE))
	{
		command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;
	}            
	else
	{
		command.responseErrorFlags = 0;
	} 	
	
	
    content.command = &command;
	if(cmd->data)
	{
        if ((cmd->cmd_code == WRITE_BLOCK) || (cmd->cmd_code == WRITE_MULTIPLE_BLOCK) || 
			(cmd->cmd_code == READ_SINGLE_BLOCK) || (cmd->cmd_code == READ_MULTIPLE_BLOCK) )
		{
			data.enableAutoCommand12 = true;
		}            
        else
		{
			data.enableAutoCommand12 = false;
		}            

        data.enableAutoCommand23 = false;		
		
        data.enableIgnoreError = false;
        data.dataType = kUSDHC_TransferDataNormal;   
        data.blockSize = cmd->data->blksize;
        data.blockCount = cmd->data->blks;		
		LOG_D(" blksize:%d, blks:%d ", data.blockSize, data.blockCount);
		
		if (((uint32_t)cmd->data->buf) & (sizeof(uint32_t) - 1U)) /* memory alignment */ 
		{
			buf = rt_malloc_align(data.blockSize * data.blockCount, FSL_FEATURE_L1DCACHE_LINESIZE_BYTE);
			RT_ASSERT(buf != RT_NULL);			
		}
		
		if ((cmd->cmd_code == WRITE_BLOCK) || (cmd->cmd_code == WRITE_MULTIPLE_BLOCK))
		{			
			if(buf)
			{
				rt_memcpy((uint8_t *)buf, cmd->data->buf, data.blockSize * data.blockCount);
				data.txData = (uint32_t *)buf;			
			}
			else
			{
				data.txData = (uint32_t *)cmd->data->buf;
			}
			data.rxData = RT_NULL;			
		}
		else /* READ_SINGLE_BLOCK or READ_MULTIPLE_BLOCK */
		{
			if(buf)
			{
				data.rxData = (uint32_t *)buf;	
			}
			else
			{
				data.rxData = (uint32_t *)cmd->data->buf;	
			}
			data.txData = RT_NULL;			
		}
		
		content.data = &data;		
	}
	else
	{
		data.txData = NULL;
		data.rxData = NULL;
		content.data = NULL;
	}

	if (kStatus_Success != card->sdhost.transfer(card->sdhost.base, &content))
	{
		LOG_E("USDHC send cmd %d failed\n", command.index);
		cmd->err = -RT_ERROR;
	}
	
	if(buf)
	{
		if(data.rxData)
		{
			rt_memcpy(cmd->data->buf, data.rxData, data.blockSize * data.blockCount);
		}
		rt_free_align(buf);
	}
	
	if((cmd->flags & RESP_MASK) == RESP_R2)
	{
        cmd->resp[3] = command.response[0];
        cmd->resp[2] = command.response[1];
        cmd->resp[1] = command.response[2];
        cmd->resp[0] = command.response[3];
        LOG_D(" resp 0x%08X 0x%08X 0x%08X 0x%08X\n", cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3]);	                 	
	}
	else
	{
        cmd->resp[0] = command.response[0];
        LOG_D(" resp 0x%08X\n", cmd->resp[0]);	
	}
	
	mmcsd_req_complete(host);
}


static void rt_ushdc_set_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
	rt_sd_card_t *card;
	rt_uint8_t bus_width;
	
	card = (rt_sd_card_t *)host->private_data;
	
	switch(io_cfg->bus_width)
	{
		case MMCSD_BUS_WIDTH_1:
			bus_width = kSDMMCHOST_DATABUSWIDTH1BIT;
			break;
		case MMCSD_BUS_WIDTH_4:
			bus_width = kSDMMCHOST_DATABUSWIDTH4BIT;
			break;
		case MMCSD_BUS_WIDTH_8:
			bus_width = kSDMMCHOST_DATABUSWIDTH8BIT;
			break;
		default:
			bus_width = kSDMMCHOST_DATABUSWIDTH4BIT;
			break;			
	}
	LOG_D("clock:%d, bus:%d\n", io_cfg->clock, bus_width==1? 4:1 );

	if(io_cfg->power_mode == MMCSD_POWER_UP)
	{
		/* power on the card */
		SD_PowerOnCard(card->sdhost.base, card->usrParam.pwr);			
		/* set DATA bus width */
		SDMMCHOST_SET_CARD_BUS_WIDTH(card->sdhost.base, bus_width);	
	}
	else if(io_cfg->power_mode == MMCSD_POWER_ON)
	{
		/*set card freq to setclock*/
		SDMMCHOST_SET_CARD_CLOCK(card->sdhost.base, card->sdhost.sourceClock_Hz, io_cfg->clock);
		/* send card active */
		SDMMCHOST_SEND_CARD_ACTIVE(card->sdhost.base, 100U);
		/* Get host capability. */
		GET_SDMMCHOST_CAPABILITY(card->sdhost.base, &(card->sdhost.capability));
		/* set DATA bus width */
		SDMMCHOST_SET_CARD_BUS_WIDTH(card->sdhost.base, bus_width);		
	}
	else if(io_cfg->power_mode == MMCSD_POWER_OFF)
	{
		/* set DATA bus width */
		SDMMCHOST_SET_CARD_BUS_WIDTH(card->sdhost.base, bus_width);		
		/* power off card */
		SD_PowerOffCard(card->sdhost.base, card->usrParam.pwr);	
	}
	else
	{
	
	}
}

static rt_int32_t rt_ushdc_get_card_status(struct rt_mmcsd_host *host)
{
	if(s_cardInserted)
	{
		LOG_D("Get SD Card Status: Inserted\n");
		return 1;
	}
	else
	{
		LOG_D("Get SD Card Status: Removed\n");
		return 0;
	}
}

static void rt_ushdc_enable_sdio_irq(struct rt_mmcsd_host *host, rt_int32_t en)
{
	if(en)
    {
        LOG_D("enable sdio irq\n");
    }
    else
    {
        LOG_D("disable sdio irq\n");
    }
}

static const struct rt_mmcsd_host_ops ops =
{
    rt_ushdc_request,
    rt_ushdc_set_iocfg,
    rt_ushdc_get_card_status, 
    rt_ushdc_enable_sdio_irq, 
};

static rt_int32_t rt_hw_ushdc_init(void *pSD)
{
	sd_card_t *sd_card;
	
	sd_card = (sd_card_t *)pSD;
	
    /*configure system pll PFD2 fractional divider to 0x18*/
    CLOCK_InitSysPfd(kCLOCK_Pfd2, 0x18U);
    /* Configure USDHC clock source and divider */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 0U);
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 1U);	
	
	sd_card->sdhost.base = SD_HOST_BASEADDR;
    sd_card->sdhost.sourceClock_Hz = SD_HOST_CLK_FREQ;
	sd_card->usrParam.cd = &s_sdCardDetect;
	sd_card->usrParam.pwr = RT_NULL;
	
	NVIC_SetPriority(SD_HOST_IRQ,  5U);
	
    /* SD host init function */
    if (SD_HostInit(sd_card) != kStatus_Success)
    {
        LOG_E("SD host init fail.\n");
        return -RT_ERROR;
    }
	else
	{
		LOG_D("SD host init success.\n");
	}	
	return RT_EOK;
}

sd_card_t *ret_card(void)
{
	return (sd_card_t *)sd_card;
}

rt_int32_t rt_usdhc_init(void)
{
	host = mmcsd_alloc_host();
	if(!host)
	{
		LOG_E("SD Host alloc failed");
		return -RT_ERROR;
	}
	
	sd_card = rt_malloc(sizeof(rt_sd_card_t));
	if (!sd_card)
	{
		LOG_E("SD sd_card alloc failed");
		return -RT_ERROR;		
	}
	rt_memset(sd_card, 0, sizeof(rt_sd_card_t));
	
	if(rt_hw_ushdc_init(sd_card) == -RT_ERROR)
	{
		LOG_E("SD HW init failed");
		return -RT_ERROR;
	}
	
	host->ops = &ops;
	host->freq_min = SD_FREQ_MIN;
	host->freq_max = SD_FREQ_MAX;
	host->valid_ocr = VDD_32_33 | VDD_33_34;
	host->flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED;	
	
	host->max_seg_size = 65536;
	host->max_dma_segs = 1;
	host->max_blk_size = 512;
	host->max_blk_count = 4096;
	
	/* link up host and sdio */
	sd_card->host = host;
	host->private_data = sd_card;
	
	
	return 0;
}


INIT_DEVICE_EXPORT(rt_usdhc_init);

