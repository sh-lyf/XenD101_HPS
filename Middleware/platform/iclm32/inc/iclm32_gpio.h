/**
  ******************************************************************************
  * @file    iclm32_gpio.h
  * @author  iclm team
  * @brief   gpio header file
  ******************************************************************************
  */
#ifndef __ICLM32_GPIO_H__
#define __ICLM32_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "iclm32e2x_libopt.h"

typedef enum 
{
    LED0 = 0,
    LED1 = 1,
    LED_MAX
}led_typedef_enum;

#define LED0_PIN                        GPIO_PIN_0
#define LED0_GPIO_PORT                  GPIOA
#define LED1_PIN                        GPIO_PIN_1
#define LED1_GPIO_PORT                  GPIOA

#define RADAR_DATA_RECV_LED             LED0
#define RADAR_DATA_INDEX_ERR_LED        LED1
#define RADAR_DATA_RECV_OVER_FLOW_LED   LED_MAX
#define RADAR_DATA_SEND_OVER_FLOW_LED   LED_MAX
#define CMD_DATA_RECV_OVER_FLOW_LED     LED_MAX

void LED_On(led_typedef_enum lednum);
void LED_Off(led_typedef_enum lednum);
void LED_Toggle(led_typedef_enum lednum);
void LED_Init(void);

#ifdef __cplusplus
}
#endif

#endif


