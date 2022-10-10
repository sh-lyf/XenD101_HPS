/**
  ******************************************************************************
  * @file    p1f031_fpga_1.h
	* @author  MCU SYSTEM Team
	* @Version V1.0.0
  * @Date    2020-10-19
  * @brief   main function
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PY32F003_START_KIT_H
#define PY32F003_START_KIT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f003_hal.h"

/** @addtogroup BSP
  * @{
  */
	
/** @defgroup PY32F003_FPGA_1 PY32F003_FPGA_1
  * @brief This section contains the exported types, contants and functions
  *        required to use the Nucleo 32 board.
  * @{
  */

/** @defgroup PY32F003_FPGA_1_Exported_Types Exported Types
  * @{
  */	
	
typedef enum
{
  LED3 = 0,
  LED_GREEN = LED3
} Led_TypeDef;

typedef enum 
{  
  BUTTON_USER = 0,
  /* Alias */
  BUTTON_KEY  = BUTTON_USER
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef; 

#define LEDn                               1

#define LED3_PIN                           GPIO_PIN_0	//GPIO_PIN_11	
#define LED3_GPIO_PORT                     GPIOA
#define LED3_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)    do {LED3_GPIO_CLK_ENABLE(); } while(0U)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)   LED3_GPIO_CLK_DISABLE())

#define BUTTONn                            1

/**
  * @brief User push-button
  */
#define USER_BUTTON_PIN                         GPIO_PIN_12
#define USER_BUTTON_GPIO_PORT                   GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn                   EXTI4_15_IRQn
/* Aliases */
#define KEY_BUTTON_PIN                          USER_BUTTON_PIN
#define KEY_BUTTON_GPIO_PORT                    USER_BUTTON_GPIO_PORT
#define KEY_BUTTON_GPIO_CLK_ENABLE()            USER_BUTTON_GPIO_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()           USER_BUTTON_GPIO_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                    USER_BUTTON_EXTI_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == 0) USER_BUTTON_GPIO_CLK_ENABLE();} while(0)
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? USER_BUTTON_GPIO_CLK_DISABLE() : 0)

uint32_t         BSP_GetVersion(void);

/** @defgroup LED_Functions LED Functions
  * @{
  */ 

void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
void             BSP_LED_Toggle(Led_TypeDef Led);    

void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);  

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* PY32F003_START_KIT_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
