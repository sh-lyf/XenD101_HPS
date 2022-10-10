/**
  ******************************************************************************
  * @file    py32_pwr.c
  * @author  liujinyang@puyasemi.com
  * @brief   power services
  ******************************************************************************
  */
#include "platform.h"

void PWR_WakeUpInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

	//PA0-wakeup stop mode
	GPIO_InitStructure.Pin = WAKEUP_IO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(WAKEUP_IO_PORT, &GPIO_InitStructure);

	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
    
}


/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    //开启HSI,HSE,LSE,LSI,PLL所有时钟
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;     //配置HSI输出时钟为24MHz
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                              //HSI分频后作为HSISYS输出给系统时钟
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
		printf("%s Failed @%d\r\n",__func__,__LINE__);
		while(1);
    }

    //初始化CPU,AHB,APB总线时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
		printf("%s Failed @%d\r\n",__func__,__LINE__);
		while(1);
    }
}

void PWR_EnterSleepMode(void)
{
	/*Suspend Tick increment to prevent wakeup by Systick interrupt. 
	Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
	HAL_SuspendTick();

	/* Request to enter SLEEP mode */
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	/* Resume Tick interrupt if disabled prior to sleep mode entry*/
	HAL_ResumeTick();


	/* Inserted Delay */
	HAL_Delay(200);

}

void PWR_EnterStopMode(void)
{
	/* Request to enter SLEEP mode */
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	/* Inserted Delay */
	HAL_Delay(200);
}
