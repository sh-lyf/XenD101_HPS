/**
  ******************************************************************************
  * @file    iclm32_uart.h
  * @author  iclm team
  * @brief   uart header file
  ******************************************************************************
  */
#ifndef __ICLM32_UART_H__
#define __ICLM32_UART_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "iclm32e2x.h"

#define USART0_BAUDRATE                    (256000U)

#define CMD_RECV_BUF_SIZE                  (64)
#define CMD_RECV_TIMEOUT                   (10) /*ms*/

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


#ifdef __cplusplus
}
#endif

#endif

