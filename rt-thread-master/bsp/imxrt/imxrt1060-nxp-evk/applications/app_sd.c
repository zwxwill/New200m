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

#ifdef RT_USING_DEVICE
#include <rtdevice.h>
#endif

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  app_led
  * @param  thread parameter
  * @retval None
  */
void app_sd(void* parameter)
{
//	rt_hw_sd_Init();
	
	while(1)
	{

        rt_thread_mdelay(500);

	}
}












/********************************************************************************/



