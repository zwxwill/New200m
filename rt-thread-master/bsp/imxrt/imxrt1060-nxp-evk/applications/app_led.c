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
#include "drv_gpio.h"

#ifdef RT_USING_DEVICE
#include <rtdevice.h>
#endif

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* defined the LED pin: GPIO1_IO9 */
#define LED0_PIN               GET_PIN(1, 9)
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  app_led
  * @param  thread parameter
  * @retval None
  */
void app_led(void* parameter)
{
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);	
	
	while(1)
	{
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
	}
}












/********************************************************************************/




