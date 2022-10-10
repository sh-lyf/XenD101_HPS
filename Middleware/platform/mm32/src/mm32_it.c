/**
  ******************************************************************************
  * @file    mm32_it.c
  * @author  ting.gao@iclegend.com
  * @brief   interrupt services
  ******************************************************************************
  */

#include "mm32_it.h"
#include "platform.h"
#include "dataprocess.h"
#include "radar.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1);
}

void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1);
}

void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1);
}

void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1);
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    delay_decrement();
    UART_RecvTimeout();
}

#ifdef SUPPORT_LOW_POWER
void TIM3_IRQHandler(void)
{
    static uint16_t timer3Cnt = 0;

    timer3Cnt++;
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if (timer3Cnt >= POWER_DOWN_PERIOD)
    {
        TIM3_Disable();
        timer3Cnt = 0;
        Radar_EnterNormalMode();
    }
}
#endif

void SPI1_IRQHandler (void)
{
    static uint16_t cnt = 0;
    
    if(SPI_GetITStatus(SPI, SPI_IT_RX)) 
    {
        while(SPI_GetFlagStatus(SPI, SPI_FLAG_RXAVL))
        {
            g_dataRecvBuf[CHANNEL_0][cnt++] = SPI_ReceiveData(SPI);

            if (cnt >= (RadarPara.dataLen << 1))
            {
                g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1] = 1;
                cnt = 0;
            }
            else if (cnt == RadarPara.dataLen)
            {
                g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0] = 1;
            }
        }
        SPI_ClearITPendingBit(SPI, SPI_IT_RX);
    }
}

void UART1_IRQHandler(void)
{
    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET) 
    {
        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        UART_Recv();
    }
}

//#ifdef SUPPORT_ABD
#if 0
void EXTI0_1_IRQHandler(void)
{
    volatile uint16_t extiDelay = EXTI_DELAY;
    
    while(extiDelay--);
    ABD_PrintStatus();
    EXTI_ClearFlag(TSPI_EXTI_LINE);
}

void EXTI4_15_IRQHandler(void)
{
    volatile uint16_t extiDelay = EXTI_DELAY;
    
    while(extiDelay--);
    ABD_PrintStatus();
    EXTI_ClearFlag(TSPI_EXTI_LINE);
}
#endif


