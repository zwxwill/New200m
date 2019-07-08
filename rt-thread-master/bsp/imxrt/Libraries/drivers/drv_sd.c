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

#define DRV_DEBUG
#define LOG_TAG             "drv.sd"
#include <drv_log.h>

/*! @brief Card semaphore  */
static rt_sem_t s_CardAccessSemaphore = NULL;
static rt_sem_t s_CardDetectSemaphore = NULL;

/*! @brief Card descriptor. */
static sd_card_t g_sd;

/*! @brief SD card detect flag  */
static bool s_cardInserted = false;

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
            SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
        }

        /* take card detect semaphore */
        rt_sem_take(s_CardDetectSemaphore, RT_WAITING_FOREVER);
        if (s_cardInserted)
        {
            rt_kprintf("\r\nCard inserted.\r\n");
            /* power on the card */
            SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
            /* Init card. */
            if (SD_CardInit(&g_sd))
            {
                LOG_E("\r\nSD card init failed.\r\n");
                return;
            }
            rt_sem_release(s_CardAccessSemaphore);
        }
        else
        {
            rt_kprintf("\r\nPlease insert a card into board.\r\n");
        }
    }
}

int rt_hw_sd_Init(void)
{
    int result = RT_EOK;
	
    /*configure system pll PFD2 fractional divider to 18*/
    CLOCK_InitSysPfd(kCLOCK_Pfd0, 0x12U);
    /* Configure USDHC clock source and divider */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 0U);
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 1U);

    s_CardAccessSemaphore = rt_sem_create("SDCardAccessSem", 0, RT_IPC_FLAG_FIFO);
    s_CardDetectSemaphore = rt_sem_create("SDCardDetectSem", 0, RT_IPC_FLAG_FIFO);
	if ((s_CardAccessSemaphore == RT_NULL) || (s_CardAccessSemaphore == RT_NULL))
	{
		rt_kprintf("create dynamic semaphore failed.\n");
		return -1;
	}
	else
	{
		rt_kprintf("create done. dynamic semaphore value = 0.\n");
	}
	
    g_sd.host.base = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd = &s_sdCardDetect;

	NVIC_SetPriority(SD_HOST_IRQ,  5U);
	
//    /* SD host init function */
//    if (SD_HostInit(&g_sd) != kStatus_Success)
//    {
//        LOG_E("\r\nSD host init fail\r\n");
//        return -1;
//    }
	
	return result;
}
//MSH_CMD_EXPORT(rt_hw_sd_Init, rt_hw_sd_Init);
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
	CardDetectTask(0);
}
MSH_CMD_EXPORT(sd_sample, sd_sample);

#endif  /* FINSH_USING_MSH */
#endif  /* DRV_DEBUG */
#endif  /* BSP_USING_SD */



