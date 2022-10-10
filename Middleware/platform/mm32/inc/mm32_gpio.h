/**
  ******************************************************************************
  * @file    mm32_gpio.h
  * @author  ting.gao@iclegend.com
  * @brief   gpio header file
  ******************************************************************************
  */
#ifndef __MM32_GPIO_H__
#define __MM32_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "hal_conf.h"
#include "global_conf.h"

typedef enum 
{
    LED0 = 0, //LD2
    LED_MAX
}led_typedef_enum;

#define LED0_GPIO_CLK                   RCC_AHBENR_GPIOA
#define LED0_GPIO_PORT                  GPIOA
#define LED0_PIN                        GPIO_Pin_6

#if defined(MM32_EVAL_BOARD)
#define TSPI_IO_PORT                    GPIOA
#define TSPI_IO_PIN                     GPIO_Pin_11
#define TSPI_EXTI_PORT_SOURCE           EXTI_PortSourceGPIOA
#define TSPI_EXTI_PIN_SOURCE            EXTI_PinSource11
#define TSPI_EXTI_LINE                  EXTI_Line11
#define TSPI_EXTI_IRQn                  EXTI4_15_IRQn
#define IO_OUT_PORT                     GPIOA
#define IO_OUT_PIN                      GPIO_Pin_10
#elif defined(XenD101_1815_DEBUG_BOARD) || defined(XenD101_1815_ULP_BOARD)
#define TSPI_IO_PORT                    GPIOA
#define TSPI_IO_PIN                     GPIO_Pin_6
#define TSPI_EXTI_PORT_SOURCE           EXTI_PortSourceGPIOA
#define TSPI_EXTI_PIN_SOURCE            EXTI_PinSource6
#define TSPI_EXTI_LINE                  EXTI_Line6
#define TSPI_EXTI_IRQn                  EXTI4_15_IRQn
#define IO_OUT_PORT                     GPIOA
#define IO_OUT_PIN                      GPIO_Pin_0
#elif defined(XenD101_1815_2_BOARD)
#ifdef SUPPORT_NEW_PIN
#define TSPI_IO_PORT                    GPIOB
#define TSPI_IO_PIN                     GPIO_Pin_1
#else
#define TSPI_IO_PORT                    GPIOA
#define TSPI_IO_PIN                     GPIO_Pin_1
#endif
#define TSPI_EXTI_PORT_SOURCE           EXTI_PortSourceGPIOA
#define TSPI_EXTI_PIN_SOURCE            EXTI_PinSource1
#define TSPI_EXTI_LINE                  EXTI_Line1
#define TSPI_EXTI_IRQn                  EXTI0_1_IRQn
#define IO_OUT_PORT                     GPIOA
#ifdef SUPPORT_NEW_PIN
#define IO_OUT_PIN                      GPIO_Pin_1
#else
#define IO_OUT_PIN                      GPIO_Pin_6
#endif
#endif

#define PWCTL_GPIO_CLK                   RCC_AHBENR_GPIOA
#define PWCTL_GPIO_PORT                  GPIOA
#define PWCTL_GPIO_PIN                   GPIO_Pin_7

#define RADAR_DATA_RECV_LED             LED0
#define RADAR_DATA_INDEX_ERR_LED        LED_MAX
#define RADAR_DATA_RECV_OVER_FLOW_LED   LED_MAX
#define RADAR_DATA_SEND_OVER_FLOW_LED   LED_MAX
#define CMD_DATA_RECV_OVER_FLOW_LED     LED_MAX

#define ABD_CHECK_INTERVAL              (10)

void LED_On(led_typedef_enum lednum);
void LED_Off(led_typedef_enum lednum);
void LED_Toggle(led_typedef_enum lednum);
void LED_Init(void);
void IoOutInit(void);
void IoOutOn(void);
void IoOutOff(void);
void TspiIoInit(void);

void GPIO_PowerCtlPinInit(void);
void GPIO_PowerCtlOn(void);
void GPIO_PowerCtlOff(void);

#ifdef __cplusplus
}
#endif

#endif

