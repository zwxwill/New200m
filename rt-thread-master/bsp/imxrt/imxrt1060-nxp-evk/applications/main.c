/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-29     tyustli      first version
 */  

#include <rtdevice.h>
 
  
extern void app_led(void* parameter);
extern void app_sd(void* parameter);

void app_create(void)
{
	rt_thread_t tid = RT_NULL;
	
	/* led thread */
    tid = rt_thread_create("app_led",   /* thread name */
                            app_led,    /* thread entry */
	                        (void*)0,   /* thread parameter */
							512,        /* thread stack size : byte */
                            RT_THREAD_PRIORITY_MAX - 5, /* thread priority : 0 is the max high priority */
							5); /* thread tick : when thread priority is same */
    if (tid != RT_NULL)
        rt_thread_startup(tid);		
	
	/* sd thread */
    tid = rt_thread_create("app_sd",   /* thread name */
                            app_sd,    /* thread entry */
	                        (void*)0,   /* thread parameter */
							2048,        /* thread stack size : byte */
                            RT_THREAD_PRIORITY_MAX - 3, /* thread priority : 0 is the max high priority */
							5); /* thread tick : when thread priority is same */
    if (tid != RT_NULL)
        rt_thread_startup(tid);	
}

int main(void)
{
	/* application create */
	app_create();
	
    while (1)
    {
        rt_thread_mdelay(500);
    }
}

