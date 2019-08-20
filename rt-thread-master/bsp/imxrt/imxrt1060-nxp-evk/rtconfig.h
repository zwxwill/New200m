#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 256
#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_MEMHEAP_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 512
#define RT_CONSOLE_DEVICE_NAME "uart1"
#define RT_VER_NUM 0x40002

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 3

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 16
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_CPUTIME
#define RT_USING_PIN
#define RT_USING_RTC
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE 512
#define RT_SDIO_THREAD_PRIORITY 15
#define RT_MMCSD_STACK_SIZE 1024
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION 16

/* Using Hardware Crypto drivers */


/* Using WiFi */


/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_POSIX

/* Network */

/* Socket abstraction layer */

#define RT_USING_SAL

/* protocol stack implement */

#define SAL_USING_LWIP
#define SAL_USING_TLS
#define SAL_USING_POSIX

/* Network interface device */

#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT

/* light weight TCP/IP stack */

#define RT_USING_LWIP
#define RT_USING_LWIP210
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_DNS
#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "192.168.1.30"
#define RT_LWIP_GWADDR "192.168.1.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 4
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 2048
#define RT_LWIP_ETHTHREAD_PRIORITY 12
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_NETIF_LOOPBACK 0
#define RT_LWIP_USING_PING

/* Modbus master and slave stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
#define ULOG_USING_ISR_LOG
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128
#define ULOG_USING_ASYNC_OUTPUT
#define ULOG_ASYNC_OUTPUT_BUF_SIZE 2048
#define ULOG_ASYNC_OUTPUT_BY_THREAD
#define ULOG_ASYNC_OUTPUT_THREAD_STACK 2048
#define ULOG_ASYNC_OUTPUT_THREAD_PRIORITY 30

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_USING_FILTER
#define RT_USING_UTEST
#define UTEST_THR_STACK_SIZE 4096
#define UTEST_THR_PRIORITY 20

/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_PAHOMQTT
#define PAHOMQTT_PIPE_MODE
#define PKG_USING_PAHOMQTT_TEST
#define MQTT_USING_TLS
#define RT_PKG_MQTT_THREAD_STACK_SIZE 6144
#define PKG_PAHOMQTT_SUBSCRIBE_HANDLERS 1
#define MQTT_DEBUG
#define PKG_USING_PAHOMQTT_LATEST
#define PKG_USING_WEBNET
#define WEBNET_PORT 80
#define WEBNET_CONN_MAX 16
#define WEBNET_ROOT "/webnet"

/* Select supported modules */

#define WEBNET_USING_AUTH
#define WEBNET_USING_CGI
#define WEBNET_USING_ASP
#define WEBNET_USING_SSI
#define WEBNET_USING_INDEX
#define WEBNET_USING_ALIAS
#define WEBNET_USING_UPLOAD
#define WEBNET_CACHE_LEVEL 0
#define WEBNET_USING_SAMPLES
#define PKG_USING_WEBNET_LATEST_VERSION
#define PKG_USING_CJSON
#define PKG_USING_CJSON_LATEST_VERSION

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */

#define PKG_USING_NETUTILS
#define PKG_NETUTILS_TFTP
#define NETUTILS_TFTP_PORT 69
#define PKG_USING_NETUTILS_LATEST_VERSION

/* IoT Cloud */


/* security packages */

#define PKG_USING_MBEDTLS

/* Select Root Certificate */

#define PKG_USING_MBEDTLS_USER_CERTS
#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ECP_WINDOW_SIZE 2
#define MBEDTLS_SSL_MAX_CONTENT_LEN 3584
#define PKG_USING_MBEDTLS_V260

/* language packages */


/* multimedia packages */


/* tools packages */

#define PKG_USING_SYSTEMVIEW
#define PKG_SYSVIEW_APP_NAME "RT-Thread Trace"
#define PKG_SYSVIEW_DEVICE_NAME "Cortex-M7"
#define PKG_SYSVIEW_TIMESTAMP_FREQ 0
#define PKG_SYSVIEW_CPU_FREQ 0
#define PKG_SYSVIEW_RAM_BASE 0x20000000
#define PKG_SYSVIEW_EVENTID_OFFSET 32
#define PKG_SYSVIEW_USE_CYCCNT_TIMESTAMP
#define PKG_SYSVIEW_SYSDESC0 "I#15=SysTick"
#define PKG_SYSVIEW_SYSDESC1 ""
#define PKG_SYSVIEW_SYSDESC2 ""

/* Segger RTT configuration */

#define PKG_SEGGER_RTT_MAX_NUM_UP_BUFFERS 3
#define PKG_SEGGER_RTT_MAX_NUM_DOWN_BUFFERS 3
#define PKG_SEGGER_RTT_BUFFER_SIZE_UP 1024
#define PKG_SEGGER_RTT_BUFFER_SIZE_DOWN 16
#define PKG_SEGGER_RTT_PRINTF_BUFFER_SIZE 64
#define PKG_SEGGER_RTT_AS_SERIAL_DEVICE
#define PKG_SERIAL_DEVICE_NAME "segger"
#define PKG_SEGGER_RTT_MODE_ENABLE_NO_BLOCK_SKIP
#define PKG_SEGGER_RTT_MAX_INTERRUPT_PRIORITY 0x20

/* SystemView buffer configuration */

#define PKG_SEGGER_SYSVIEW_RTT_BUFFER_SIZE 1024
#define PKG_SEGGER_SYSVIEW_RTT_CHANNEL 1
#define PKG_SEGGER_SYSVIEW_USE_STATIC_BUFFER

/* SystemView Id configuration */

#define PKG_SEGGER_SYSVIEW_ID_BASE 0x10000000
#define PKG_SEGGER_SYSVIEW_ID_SHIFT 2
#define PKG_USING_SYSTEMVIEW_LATEST_VERSION

/* system packages */


/* peripheral libraries and drivers */

/* sensors drivers */


/* miscellaneous packages */


/* samples: kernel and components samples */


/* Hardware Drivers Config */

#define SOC_IMXRT1062CVL5B

/* On-chip Peripheral Drivers */

#define BSP_USING_DMA
#define BSP_USING_GPIO
#define BSP_USING_SDRAM
#define BSP_USING_LPUART
#define BSP_USING_LPUART1
#define BSP_LPUART1_RX_USING_DMA
#define BSP_LPUART1_RX_DMA_CHANNEL 0
#define BSP_USING_RTC
#define BSP_USING_RNG
#define BSP_USING_SD
#define BSP_USING_ETH

/* Onboard Peripheral Drivers */

#define BSP_USING_SDCARD

/* Board extended module Drivers */


#endif
