/**
  ******************************************************************************
  * @file    mm32_it.h
  * @author  ting.gao@iclegend.com
  * @brief   interrupt header file
  ******************************************************************************
  */

#ifndef __MM32_IT_H__
#define __MM32_IT_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#define EXTI_DELAY                  (1000)

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void UART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
