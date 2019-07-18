/**
  *****************************************************************************
  * @file    app_led.c
  * @author  zwx
  * @version V1.0.0
  * @date    2019-05-30
  * @brief   
  ******************************************************************************
  */  
#include "rtthread.h"
#include "drv_sd.h"
#include "dfs_fs.h"

#ifdef RT_USING_DEVICE
#include <rtdevice.h>
#endif

//#define DRV_DEBUG
#define LOG_TAG             "app.sd"
#include <drv_log.h>


/* Exported variables --------------------------------------------------------*/
uint8_t g_uiFileOk = 0;
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SD_WAIT_TIME_MAX   5000  /* 5Second */
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  app_led
  * @param  thread parameter
  * @retval None
  */
void app_sd(void* parameter)
{
	rt_uint32_t result;
	
	/* Check if the SD card is inserted */
	result = mmcsd_wait_cd_changed(SD_WAIT_TIME_MAX);
	if(result == MMCSD_HOST_PLUGED)
	{
		LOG_D("SD Card is inserted.\n");
        /* mount sd card fat partition 1 as root directory */
        if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
        {
		    LOG_I("File System initialized!\n");
			g_uiFileOk = 1;
		}        
        else
        {
			LOG_E("File System init failed!\n");
		}            		
	}
	else
	{
		LOG_E("SD Card init faild or timeout: %d!", result);
	}
	
	while(1)
	{

        rt_thread_mdelay(500);

	}
}












/********************************************************************************/




