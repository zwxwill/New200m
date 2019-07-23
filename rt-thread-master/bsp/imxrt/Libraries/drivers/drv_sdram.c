/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rtthread.h>
#ifdef BSP_USING_SDRAM

#include "sdram_port.h"
#include "board.h"
#include "fsl_semc.h"
#include "drv_sdram.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"

#define DRV_DEBUG
#define LOG_TAG             "drv.sdram"
#include <drv_log.h>

#ifdef RT_USING_MEMHEAP_AS_HEAP
static struct rt_memheap system_heap;
#endif

int rt_hw_sdram_Init(void)
{
    int result = RT_EOK;
    semc_config_t config;
    semc_sdram_config_t sdramconfig;
    rt_uint32_t clockFrq = CLOCK_GetFreq(kCLOCK_SemcClk);
	
	IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_39_SEMC_DQS,            /* GPIO_EMC_39 is configured as SEMC_DQS */
      1U); 	/* modify by [zwx], because mcupress config tool can't select as input function in this pin */

	IOMUXC_SetPinConfig(
	  IOMUXC_GPIO_EMC_29_SEMC_CS0,            /* GPIO_EMC_29 PAD functional properties : */
	  0x0110F9U);                             /* Slew Rate Field: Fast Slew Rate
												 Drive Strength Field: R0/7
												 Speed Field: max(200MHz)
												 Open Drain Enable Field: Open Drain Disabled
												 Pull / Keep Enable Field: Pull/Keeper Enabled
												 Pull / Keep Select Field: Keeper
												 Pull Up / Down Config. Field: 100K Ohm Pull Down
												 Hyst. Enable Field: Hysteresis Enabled */	
	
    /* Initializes the MAC configure structure to zero. */
    memset(&config, 0, sizeof(semc_config_t));
    memset(&sdramconfig, 0, sizeof(semc_sdram_config_t));
     
    /* Initialize SEMC. */
    SEMC_GetDefaultConfig(&config);
    config.dqsMode = kSEMC_Loopbackdqspad;  /* For more accurate timing. */
    SEMC_Init(SEMC, &config);

    /* Configure SDRAM. */
    sdramconfig.csxPinMux               = SDRAM_CS_PIN;
    sdramconfig.address                 = SDRAM_BANK_ADDR;
    sdramconfig.memsize_kbytes          = SDRAM_SIZE;
    sdramconfig.portSize                = SDRAM_DATA_WIDTH;
    sdramconfig.burstLen                = kSEMC_Sdram_BurstLen8;
    sdramconfig.columnAddrBitNum        = SDRAM_COLUMN_BITS;
    sdramconfig.casLatency              = SDRAM_CAS_LATENCY;
    sdramconfig.tPrecharge2Act_Ns       = SDRAM_TRP;
    sdramconfig.tAct2ReadWrite_Ns       = SDRAM_TRCD;
    sdramconfig.tRefreshRecovery_Ns     = SDRAM_REFRESH_RECOVERY;
    sdramconfig.tWriteRecovery_Ns       = SDRAM_TWR;
    sdramconfig.tCkeOff_Ns              = 42;  /* The minimum cycle of SDRAM CLK off state. CKE is off in self refresh at a minimum period tRAS.*/
    sdramconfig.tAct2Prechage_Ns        = SDRAM_TRAS;
    sdramconfig.tSelfRefRecovery_Ns     = 67;
    sdramconfig.tRefresh2Refresh_Ns     = SDRAM_TRC;
    sdramconfig.tAct2Act_Ns             = SDRAM_ACT2ACT;
    sdramconfig.tPrescalePeriod_Ns      = 160 * (1000000000 / clockFrq);
    sdramconfig.refreshPeriod_nsPerRow  = SDRAM_REFRESH_ROW;
    sdramconfig.refreshUrgThreshold     = sdramconfig.refreshPeriod_nsPerRow;
    sdramconfig.refreshBurstLen         = 1;
    result = SEMC_ConfigureSDRAM(SEMC, SDRAM_REGION, &sdramconfig, clockFrq);
    if(result != kStatus_Success)
    {
        LOG_E("SDRAM init failed!\n");
        result = -RT_ERROR;
    }
    else
    {
        LOG_D("sdram init success, mapped at 0x%X, size is %d bytes.\n", SDRAM_BANK_ADDR, SDRAM_SIZE);
#ifdef RT_USING_MEMHEAP_AS_HEAP
        /* If RT_USING_MEMHEAP_AS_HEAP is enabled, SDRAM is initialized to the heap */
        rt_memheap_init(&system_heap, "sdram", (void *)SDRAM_BANK_ADDR, SDRAM_SIZE);
		LOG_D("Rt-Thread Heap(SDRAM), begin: 0x%p, end: 0x%p, size:%dk\n", SDRAM_BANK_ADDR, SDRAM_BANK_ADDR+SDRAM_SIZE, SDRAM_SIZE);
#endif
    }
    
    return result;
}
#ifndef SKIP_SYSCLK_INIT
INIT_BOARD_EXPORT(rt_hw_sdram_Init);
#endif

#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH

#define SEMC_DATALEN                (0x1000U)
rt_uint32_t sdram_writeBuffer[SEMC_DATALEN];
rt_uint32_t sdram_readBuffer[SEMC_DATALEN];

/* read write 32bit test */
void sdram_sample(void)
{
    rt_uint32_t index;
    rt_uint32_t datalen = SEMC_DATALEN;
    rt_uint32_t *sdram = (rt_uint32_t *)SDRAM_BANK_ADDR; /* SDRAM start address. */
    bool result = true;

    LOG_D("\r\n SEMC SDRAM Memory 32 bit Write Start, Start Address 0x%x, Data Length %d !\r\n", sdram, datalen);
    /* Prepare data and write to SDRAM. */
    for (index = 0; index < datalen; index++)
    {
        sdram_writeBuffer[index] = index;
        sdram[index] = sdram_writeBuffer[index];
    }

    LOG_D("\r\n SEMC SDRAM Read 32 bit Data Start, Start Address 0x%x, Data Length %d !\r\n", sdram, datalen);
    /* Read data from the SDRAM. */
    for (index = 0; index < datalen; index++)
    {
        sdram_readBuffer[index] = sdram[index];
    }

    LOG_D("\r\n SEMC SDRAM 32 bit Data Write and Read Compare Start!\r\n");
    /* Compare the two buffers. */
    while (datalen--)
    {
        if (sdram_writeBuffer[datalen] != sdram_readBuffer[datalen])
        {
            result = false;
            break;
        }
    }

    if (!result)
    {
        LOG_E("\r\n SEMC SDRAM 32 bit Data Write and Read Compare Failed!\r\n");
    }
    else
    {
        LOG_D("\r\n SEMC SDRAM 32 bit Data Write and Read Compare Succeed!\r\n");
    }
}
MSH_CMD_EXPORT(sdram_sample, sdram_sample)

#endif /* DRV_DEBUG */
#endif /* FINSH_USING_MSH */
#endif /* BSP_USING_SDRAM */
