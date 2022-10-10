/**
  ******************************************************************************
  * @file    platform.c
  * @author  ting.gao@iclegend.com
  * @brief   platform dependent services
  ******************************************************************************
  */
#include "platform.h"
#include "stdio.h"

void Delay(uint32_t time)
{
#if defined(GD32_PLATFORM) || defined(ICLM32_PLATFORM)
    delay_1ms(time);
#endif
#if defined(STM32_PLATFORM) || defined(PY32_PLATFORM)
    HAL_Delay(time);
#endif
}

void Indicator_RadarDataReceived(uint16_t threshold)
{
    static uint16_t recvCnt = 0;
    
    recvCnt++;
    if (recvCnt % threshold == 0)
    {
        LED_Toggle(RADAR_DATA_RECV_LED);
    }
}

void Indicator_RadarDataIndexError(void)
{
    LED_Toggle(RADAR_DATA_INDEX_ERR_LED);
}

void Indicator_RadarDataRecvOverFlow(void)
{
    LED_Toggle(RADAR_DATA_RECV_OVER_FLOW_LED);
}

void Indicator_RadarDataSendOverFlow(void)
{
    static uint8_t recvCnt = 0;
    
    recvCnt++;
    if (recvCnt % INDICATOR_SEND_OF_THRESHOLD == 0)
    {
        LED_Toggle(RADAR_DATA_SEND_OVER_FLOW_LED);
    }
}

void Indicator_CmdDataRecvOverFlow(void)
{
    LED_Toggle(CMD_DATA_RECV_OVER_FLOW_LED);
}

void Platform_Init(void)
{
#if defined (GD32_PLATFORM) || defined(MM32_PLATFORM)
    SYSTICK_Init();
#endif
#ifdef STM32_PLATFORM
    CACHE_Enable();
    HAL_Init();
    SystemClock_Config();
#endif
#ifdef ICLM32_PLATFORM
    INT_Init();
    SYSTICK_Init();
    GPIO_IntTest();
#endif
#ifdef PY32_PLATFORM
    HAL_Init();
    SystemCoreClockUpdate();
#endif

#ifdef DEVMB
	W25QXX_Init();
    QSPI_EnableMemoryMappedMode();
#endif
#ifdef SUPPORT_LOW_POWER
#ifndef MCU_LOW_POWER
    TIM3_Init();
#endif
#endif

    LED_Init();
    UART_Init();
    I2C_Init(I2C_Speed_400k);
#if defined(MM32_PLATFORM)|| defined(PY32_PLATFORM)
    IWDG_Init();
#endif
}

