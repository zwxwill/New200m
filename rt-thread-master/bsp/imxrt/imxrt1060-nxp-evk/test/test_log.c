/**
  *****************************************************************************
  * @file    test_log.c
  * @author  zwx
  * @version V1.0.0
  * @date    2019-07-17
  * @brief   
  ******************************************************************************
  */  
#include "rtthread.h"
#include "test_log.h"


//#define DRV_DEBUG
#define LOG_TAG             "test.log"
#include <drv_log.h>

extern uint8_t g_uiFileOk;

static struct ulog_backend sd_backend;

static void ulog_sdlog_backend_output(struct ulog_backend *backend, rt_uint32_t level, const char *tag, rt_bool_t is_raw,
        const char *log, size_t len)
{

	int i =0 ;
	uint8_t *buf_8 = (uint8_t *)log;
	int fd, size;
		
#if 0		
	rt_kprintf("\nflash save: ");	
	for(i=0; i<len; i++)
	{
		rt_kprintf("%c", buf_8[i]);	
	}	
	rt_kprintf("\n");	
#endif

#if 1
	if(g_uiFileOk)
	{
	    fd = open("/text.txt", O_WRONLY | O_CREAT | O_APPEND);
	    if (fd >= 0)
	    {
	        write(fd, buf_8, len);
	        close(fd);
	    }		
	}

#endif

}


int ulog_sdlog_backend_init(void)
{
    sd_backend.output = ulog_sdlog_backend_output;

    ulog_backend_register(&sd_backend, "sdlog", RT_TRUE);

    return 0;
}
INIT_APP_EXPORT(ulog_sdlog_backend_init);


















