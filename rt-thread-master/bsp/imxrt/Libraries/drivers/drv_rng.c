/*
 * Copyright 2019 ZWX
 * All rights reserved.
 *
 */

#include <rtthread.h>
#ifdef BSP_USING_RNG

#include "drv_rng.h"
#include "fsl_trng.h"
#include "pin_mux.h"
#include "clock_config.h"

#define DRV_DEBUG
#define LOG_TAG             "drv.rng"
#include <drv_log.h>

int rt_hw_rng_Init(void)
{
    int result = RT_EOK;
	trng_config_t trngConfig;
	int32_t status;
	
    TRNG_GetDefaultConfig(&trngConfig);
    /* Set sample mode of the TRNG ring oscillator to Von Neumann, for better random data.
     * It is optional.*/
    trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;
    /* Initialize TRNG */
    status = TRNG_Init(TRNG, &trngConfig);
    if (kStatus_Success != status)
	{
        LOG_E("RNG init failed!\n");
        result = -RT_ERROR;		
	}
	else
	{
		LOG_D("RNG init success.\n");
	}
	return result;
}

INIT_BOARD_EXPORT(rt_hw_rng_Init);

int rt_getRandomData(void *data, uint32_t dataSize)
{
	return TRNG_GetRandomData(TRNG, data, dataSize);
}

#define DRV_DEBUG
#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH

#define TRNG_EXAMPLE_RANDOM_NUMBER 10
void rng_sample(void)
{
	uint32_t data[TRNG_EXAMPLE_RANDOM_NUMBER];
	uint32_t i = 0;
	int32_t status;
	
	rt_kprintf("Generate %d random numbers: \r\n", TRNG_EXAMPLE_RANDOM_NUMBER);

	/* Get Random data*/
	status = rt_getRandomData(data, sizeof(data));
	if (status == kStatus_Success)
	{
		/* Print data*/
		for (i = 0; i < TRNG_EXAMPLE_RANDOM_NUMBER; i++)
		{
			rt_kprintf("Random[%d] = 0x%X\r\n", i, data[i]);
		}
	}
	else
	{
		rt_kprintf("TRNG failed!\r\n");
	}
}
MSH_CMD_EXPORT(rng_sample, rng_sample);

#endif  /* FINSH_USING_MSH */
#endif  /* DRV_DEBUG */
#endif  /* BSP_USING_RNG */





