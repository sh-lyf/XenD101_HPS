/**
  ******************************************************************************
  * @file    py32_gpio.h
  * @author  liujinyang@puyasemi.com
  * @brief   gpio header file
  ******************************************************************************
  */
#ifndef __PY32_GPIO_H__
#define __PY32_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f003_hal_conf.h"
#include "global_conf.h"

typedef enum
{
  LED0 = 0,
  LED_MAX
} Led_TypeDef;

#define LEDn                            1

#define LED0_GPIO_PORT                  GPIOA
#define LED0_PIN                        GPIO_PIN_1
#define LED0_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED0_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOA_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)    do {LED0_GPIO_CLK_ENABLE(); } while(0U)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)   LED0_GPIO_CLK_DISABLE())

#if defined(PY32_EVAL_BOARD)
#define TSPI_IO_PORT                    GPIOA
#define TSPI_IO_PIN                     GPIO_PIN_11
#define TSPI_EXTI_PORT_SOURCE           EXTI_PortSourceGPIOA
#define TSPI_EXTI_PIN_SOURCE            EXTI_PinSource11
#define TSPI_EXTI_LINE                  EXTI_Line11
#define TSPI_EXTI_IRQn                  EXTI4_15_IRQn
#define IO_OUT_PORT                     GPIOA
#define IO_OUT_PIN                      GPIO_PIN_10
#elif defined(XenD101_1815_DEBUG_BOARD) || defined(XenD101_1815_ULP_BOARD)
#define TSPI_IO_PORT                    GPIOA
#define TSPI_IO_PIN                     GPIO_PIN_6
#define TSPI_EXTI_PORT_SOURCE           EXTI_PortSourceGPIOA
#define TSPI_EXTI_PIN_SOURCE            EXTI_PinSource6
#define TSPI_EXTI_LINE                  EXTI_Line6
#define TSPI_EXTI_IRQn                  EXTI4_15_IRQn
#define IO_OUT_PORT                     GPIOA
#define IO_OUT_PIN                      GPIO_PIN_0
#elif defined(XenD101_1815_2_BOARD)
#define TSPI_IO_PORT                    GPIOA
#define TSPI_IO_PIN                     GPIO_PIN_5

#define TSPI_IO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define TSPI_IO_CLK_DISABLE()         __HAL_RCC_GPIOA_CLK_DISABLE()

#define TSPI_EXTI_PORT_SOURCE           EXTI_PortSourceGPIOA
#define TSPI_EXTI_PIN_SOURCE            EXTI_PinSource1
#define TSPI_EXTI_LINE                  EXTI_Line1
#define TSPI_EXTI_IRQn                  EXTI0_1_IRQn

#define IO_OUT_PORT                     GPIOB
#define IO_OUT_PIN                      GPIO_PIN_6
#define IO_OUT_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define IO_OUT_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()

#endif

#define RADAR_DATA_RECV_LED             LED0
#define RADAR_DATA_INDEX_ERR_LED        LED_MAX
#define RADAR_DATA_RECV_OVER_FLOW_LED   LED_MAX
#define RADAR_DATA_SEND_OVER_FLOW_LED   LED_MAX
#define CMD_DATA_RECV_OVER_FLOW_LED     LED_MAX

#define ABD_CHECK_INTERVAL              (10)

void LED_On(Led_TypeDef Led);
void LED_Off(Led_TypeDef Led);
void LED_Toggle(Led_TypeDef Led);
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

