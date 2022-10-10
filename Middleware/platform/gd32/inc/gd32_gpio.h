/**
  ******************************************************************************
  * @file    gd32_gpio.h
  * @author  ting.gao@iclegend.com
  * @brief   gpio header file
  ******************************************************************************
  */
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "gd32f3x0.h"

typedef enum 
{
    LED0 = 0,
    LED1 = 1,
    LED2 = 2,
    LED3 = 3,
    LED_MAX
}led_typedef_enum;

#define LED0_PIN                         GPIO_PIN_10
#define LED0_GPIO_PORT                   GPIOC
#define LED0_GPIO_CLK                    RCU_GPIOC

#define LED1_PIN                         GPIO_PIN_11
#define LED1_GPIO_PORT                   GPIOC
#define LED1_GPIO_CLK                    RCU_GPIOC

#define LED2_PIN                         GPIO_PIN_12
#define LED2_GPIO_PORT                   GPIOC
#define LED2_GPIO_CLK                    RCU_GPIOC

#define LED3_PIN                         GPIO_PIN_2
#define LED3_GPIO_PORT                   GPIOC
#define LED3_GPIO_CLK                    RCU_GPIOC

#define RADAR_DATA_RECV_LED             LED0
#define RADAR_DATA_INDEX_ERR_LED        LED1
#define RADAR_DATA_RECV_OVER_FLOW_LED   LED2
#define RADAR_DATA_SEND_OVER_FLOW_LED   LED3
#define CMD_DATA_RECV_OVER_FLOW_LED     LED2 /*fix me*/

void LED_On(led_typedef_enum lednum);
void LED_Off(led_typedef_enum lednum);
void LED_Toggle(led_typedef_enum lednum);
void LED_Init(void);

#ifdef __cplusplus
}
#endif

#endif

