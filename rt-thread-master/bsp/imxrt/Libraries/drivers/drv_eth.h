/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-07-19     Zhaoweixiong the first version.
 */

#ifndef DRV_ETH_H__
#define DRV_ETH_H__

#include <rtdevice.h>

#ifndef ENET_PRIORITY
    #define ENET_PRIORITY       (6U)
#endif
#ifndef ENET_1588_PRIORITY
    #define ENET_1588_PRIORITY  (5U)
#endif

#define FSL_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT

#endif


