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

#include "hal_conf.h"
#include "global_conf.h"

#define UART1_TX_PORT                    GPIOA
#define UART1_TX_PIN                     GPIO_Pin_12
#define UART1_TX_AFSOURCE                GPIO_PinSource12
#define UART1_TX_AFMODE                  GPIO_AF_1

#ifdef MM32_EVAL_BOARD
#define UART1_RX_PORT                    GPIOA
#define UART1_RX_PIN                     GPIO_Pin_0
#define UART1_RX_AFSOURCE                GPIO_PinSource0
#define UART1_RX_AFMODE                  GPIO_AF_1
#else
#define UART1_RX_PORT                    GPIOA
#define UART1_RX_PIN                     GPIO_Pin_3
#define UART1_RX_AFSOURCE                GPIO_PinSource3
#define UART1_RX_AFMODE                  GPIO_AF_1
#endif

#ifdef PRINT_BY_UART2
#define UART2_TX_PORT                    GPIOA
#define UART2_TX_PIN                     GPIO_Pin_1
#define UART2_TX_AFSOURCE                GPIO_PinSource1
#define UART2_TX_AFMODE                  GPIO_AF_2
#endif

#ifdef PRINT_BY_UART2
#define PRINT_UART                       UART2
#else
#define PRINT_UART                       UART1
#endif

#define UART_BAUDRATE                     (256000U)

#define CMD_RECV_BUF_SIZE                  (64)
#define CMD_RECV_TIMEOUT                   (10) /*ms*/

#define SEND_BUF_CNT                       (2) 

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
extern uint8_t g_uartSendTime;

void UART_Init(void);
void UART_RecvTimeout(void);
void UART_Recv(void);
void UART_Send2Buf(uint8_t* pBuffer, uint16_t uBufLen);
void UART_Send(void);
void UART_DirectSend(uint8_t* buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif

