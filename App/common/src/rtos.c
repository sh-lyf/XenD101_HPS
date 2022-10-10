/**
  ******************************************************************************
  * @file    rtos.c
  * @author  iclm team
  * @brief   rtos services
  ******************************************************************************
  */
#include "rtos.h"
#include "cmsis_os.h"
#include "dataprocess.h"
#include "usb_transfer.h"
#include "cmdprocess.h"
#ifdef STM32_PLATFORM
#include "I2CStressTest.h"
#endif
#if defined(SUPPORT_MOTION_TARGET_TRACKING) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "mtt_task.h"
#endif
#if defined(SUPPORT_GESTURE_RECOGNITION) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "gr_task.h"
#endif

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}                   

void RTOS_Init(void) 
{
    DataProc_TaskInit();
    UsbTransfer_TaskInit();
    CmdProc_TaskInit();
#ifdef STM32_PLATFORM
	I2CStressTest_TaskInit();
#endif
#if defined(SUPPORT_MOTION_TARGET_TRACKING) || defined(SUPPORT_DYNAMIC_SYS_MODE)
    MTT_TaskInit();
#endif
#if defined(SUPPORT_GESTURE_RECOGNITION) || defined(SUPPORT_DYNAMIC_SYS_MODE)
    GR_TaskInit();
#endif	
}


