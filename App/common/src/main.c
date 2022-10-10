/**
  ******************************************************************************
  * @file           : main.c
  * @author         : iclm team
  * @brief          : main program for radar dev
  ******************************************************************************
  */
#include <stdio.h>
#include "global_conf.h"
#include "platform.h"
#include "radar.h"
#include "dataprocess.h"
#include "cmdprocess.h"
#include "config.h"
#ifdef STM32_PLATFORM
#include "rtos.h"
#include "cmsis_os.h"
#endif
#ifdef SUPPORT_ABD
#include "abd.h"
#endif

int main(void)
{   
    Platform_Init();
#ifdef XenD101
    Radar_PreInit();
#endif
    //Config_Init();
    Radar_Init();
#ifdef SUPPORT_ABD
    ABD_Init();
#endif
#ifdef STM32_PLATFORM
    RTOS_Init();
#endif
    DataProc_Init(); 
    //CmdProc_Init();

#ifdef ICLM32_PLATFORM
    INT_Enable();
#endif
#ifdef XenD101
    //printf("sysclk:%d hclk:%d pclk:%d\r\n", RCC_GetSysClockFreq(), RCC_GetHCLKFreq(), RCC_GetPCLK1Freq());
#endif
    printf("system init finished!\r\n");

#ifdef STM32_PLATFORM
    osKernelStart();
    while (1);
#else
    while (1)
    {
        DataProc_Recv();
        //DataProc_Send();
        //CmdProc_Recv();        
    }
#endif
}

