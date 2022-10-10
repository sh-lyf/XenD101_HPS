/**
  ******************************************************************************
  * @file    gd32_uart.h
  * @author  ting.gao@iclegend.com
  * @brief   uart header file
  ******************************************************************************
  */
#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "gd32f3x0.h"

//#define CONFIG_SUPPORT_UART1_DEBUG

#define USART0_BAUDRATE                    (256000U)
#define USART0_DMA_CHANEL                  DMA_CH3
#define USART1_BAUDRATE                    (256000U)

#define CMD_RECV_BUF_SIZE                  (64)
#define CMD_RECV_TIMEOUT                   (10) /*ms*/

#define SEND_BUF_CNT                       (4) 

typedef enum
{
    BUF_0        = 0,
    BUF_1        = 1,
    BUF_MAX
}BufEnum;

typedef struct CMD_RECV
{
    uint8_t buf[BUF_MAX][CMD_RECV_BUF_SIZE];
    volatile uint8_t bufRecv;
    volatile uint8_t bufProc;
    volatile uint8_t cmdReady;
    volatile uint8_t idleCnt;
    volatile uint16_t curIndex;
    volatile uint16_t bufLen;
}CMD_RECV_T;

extern CMD_RECV_T g_cmdRecv;
extern volatile uint8_t g_uartDmaSendingFlag;

void UART_Init(void);
void UART0_DmaSend(uint8_t* buf, uint16_t size);
void UART_RecvTimeout(void);
void UART_Recv(void);
void UART_Send2Buf(uint8_t* pBuffer, uint16_t uBufLen);
void UART_Send(void);

#ifdef __cplusplus
}
#endif

#endif

