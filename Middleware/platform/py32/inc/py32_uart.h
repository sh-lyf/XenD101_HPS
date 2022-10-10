/**
  ******************************************************************************
  * @file    py32_uart.h
  * @author  liujinyang@puyasemi.com
  * @brief   uart header file
  ******************************************************************************
  */
#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f003_hal_conf.h"
#include "global_conf.h"

//#define UART1_TX_PORT                    GPIOA
//#define UART1_TX_PIN                     GPIO_Pin_12
//#define UART1_TX_AFSOURCE                GPIO_PinSource12
//#define UART1_TX_AFMODE                  GPIO_AF_1
//USART2
//USART2_TX PA0-AF9
//USART2_RX PF2-AF4
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_0
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF9_USART2

#define USARTx_RX_PIN                    GPIO_PIN_2
#define USARTx_RX_GPIO_PORT              GPIOF
#define USARTx_RX_AF                     GPIO_AF4_USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler


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

