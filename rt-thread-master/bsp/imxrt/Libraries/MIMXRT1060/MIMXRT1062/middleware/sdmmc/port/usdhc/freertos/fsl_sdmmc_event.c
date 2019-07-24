/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rtthread.h>
#include "fsl_sdmmc_event.h"


/*******************************************************************************
 * Definitons
 ******************************************************************************/
/*! @brief Convert the milliseconds to ticks in FreeRTOS. */
#define MSEC_TO_TICK(msec) \
    (((uint32_t)(msec) + 500uL / (uint32_t)configTICK_RATE_HZ) * (uint32_t)configTICK_RATE_HZ / 1000uL)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get event instance.
 * @param eventType The event type
 * @return The event instance's pointer.
 */
static rt_sem_t *SDMMCEVENT_GetInstance(sdmmc_event_t eventType);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Transfer complete event. */
static rt_sem_t g_eventTransferComplete;
/*! @brief Card detect event. */
static rt_sem_t g_eventCardDetect;

/*******************************************************************************
 * Code
 ******************************************************************************/
static rt_sem_t *SDMMCEVENT_GetInstance(sdmmc_event_t eventType)
{
    rt_sem_t *event;

    switch (eventType)
    {
        case kSDMMCEVENT_TransferComplete:
            event = &g_eventTransferComplete;
            break;
        case kSDMMCEVENT_CardDetect:
            event = &g_eventCardDetect;
            break;
        default:
            event = NULL;
            break;
    }

    return event;
}

bool SDMMCEVENT_Create(sdmmc_event_t eventType)
{
	static uint8_t semNo = 0;
	char name[8] = {"SDSem  "};
    rt_sem_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event)
    {
		semNo++;
		rt_sprintf(name, "SDSem%d", semNo);
        *event = rt_sem_create(name, 0, RT_IPC_FLAG_FIFO);
        if (*event == NULL)
        {
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool SDMMCEVENT_Wait(sdmmc_event_t eventType, uint32_t timeoutMilliseconds)
{
    int32_t timeoutTicks;
    rt_sem_t *event = SDMMCEVENT_GetInstance(eventType);

    if (timeoutMilliseconds && event && (*event != 0U))
    {
        if (timeoutMilliseconds == ~0U)
        {
            timeoutTicks = RT_WAITING_FOREVER;
        }
        else
        {
            timeoutTicks = rt_tick_from_millisecond(timeoutMilliseconds);
        }
        if (rt_sem_take(*event, timeoutTicks) != RT_EOK)
        {
            return false; /* timeout */
        }
        else
        {
            return true; /* event taken */
        }
    }
    else
    {
        return false;
    }
}

bool SDMMCEVENT_Notify(sdmmc_event_t eventType)
{
    rt_sem_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event && (*event != 0U))
    {
		rt_sem_release(*event);
    }
    else
    {
        return false;
    }
	return true;
}

void SDMMCEVENT_Delete(sdmmc_event_t eventType)
{
    rt_sem_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event && (*event != 0U))
    {
        rt_sem_delete(*event);
    }
}

void SDMMCEVENT_Delay(uint32_t milliseconds)
{
	rt_thread_mdelay(milliseconds);
}
