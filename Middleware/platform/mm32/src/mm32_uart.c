/**
  ******************************************************************************
  * @file    gd32_uart.c
  * @author  ting.gao@iclegend.com
  * @brief   uart services
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "mm32_uart.h"
#include "mm32_spi.h"
#include "dataprocess.h"

CMD_RECV_T g_cmdRecv;
#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
static uint8_t uartSendBuf[SEND_BUF_CNT][SPI_FRAME_LEN_MAX];
static volatile uint8_t uartSendFlag[SEND_BUF_CNT];
uint8_t g_uartSendTime = 0;
#endif

void UART1_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    
    GPIO_PinAFConfig(UART1_TX_PORT, UART1_TX_AFSOURCE, UART1_TX_AFMODE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(UART1_TX_PORT, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(UART1_RX_PORT, UART1_RX_AFSOURCE, UART1_RX_AFMODE);
    GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(UART1_RX_PORT, &GPIO_InitStructure);
}

#ifdef PRINT_BY_UART2
void UART2_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    
    GPIO_PinAFConfig(UART2_TX_PORT, UART2_TX_AFSOURCE, UART2_TX_AFMODE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = UART2_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(UART2_TX_PORT, &GPIO_InitStructure);
}
#endif

void UART_Init(void)
{
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    UART1_GPIO_Init();
    
    RCC_APB1PeriphClockCmd(RCC_APB1RSTR_UART1, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    UART_StructInit(&UART_InitStructure);
    UART_InitStructure.BaudRate = UART_BAUDRATE;
    UART_InitStructure.WordLength = UART_WordLength_8b;
    UART_InitStructure.StopBits = UART_StopBits_1;
    UART_InitStructure.Parity = UART_Parity_No;
    UART_InitStructure.HWFlowControl = UART_HWFlowControl_None;
    UART_InitStructure.Mode = UART_Mode_Rx | UART_Mode_Tx;
    HAL_UART_Init(UART1, &UART_InitStructure);
    
    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UART1, ENABLE);
#ifdef PRINT_BY_UART2
    UART2_GPIO_Init();
    RCC_APB1PeriphClockCmd(RCC_APB1RSTR_UART2, ENABLE);
    HAL_UART_Init(UART2, &UART_InitStructure);
    UART_Cmd(UART2, ENABLE);
#endif
}

void UART_RecvTimeout(void)
{
    if (g_cmdRecv.curIndex > 0)
    {
        g_cmdRecv.idleCnt++;
        if (g_cmdRecv.idleCnt >= CMD_RECV_TIMEOUT)
        {
            g_cmdRecv.cmdReady = 1;
            g_cmdRecv.bufProc = g_cmdRecv.bufRecv;
            g_cmdRecv.bufRecv = (++g_cmdRecv.bufRecv) % BUF_MAX;
            g_cmdRecv.bufLen = g_cmdRecv.curIndex;
            g_cmdRecv.curIndex = 0;
            g_cmdRecv.idleCnt = 0;
        }
    }
}

void UART_DirectSend(uint8_t* buf, uint16_t len)
{
    if (NULL == buf)
    {
        return;
    }
    
    while(len--)
    {
        UART_SendData(UART1, *buf++);
        while(!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT));
    }
}

#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
void UART_Send2Buf(uint8_t* pBuffer, uint16_t uBufLen)
{	
    static uint16_t dataIndex = 0;
    static uint8_t bufIndex = 0;
    uint16_t availableBufLen = 0;
    uint16_t copyDataLen = 0;
    uint16_t sendBufLen = DataProc_GetRadarDataLen();
    
    while (uBufLen > 0)
    {
        availableBufLen = sendBufLen - dataIndex;
        copyDataLen = availableBufLen < uBufLen ? availableBufLen : uBufLen;
        memcpy(&uartSendBuf[bufIndex][dataIndex], pBuffer, copyDataLen);
        pBuffer += copyDataLen;
        uBufLen -= copyDataLen;
        dataIndex += copyDataLen;

        if (dataIndex == sendBufLen)
        {
            uartSendFlag[bufIndex] = 1;
            bufIndex = (bufIndex + 1) % SEND_BUF_CNT;
            dataIndex = 0;
        }
    }
}

void UART_Send(void)
{   
    static uint8_t sendIdx = 0;

    if (!g_uartSendTime)
    {
        return;
    }

    while (uartSendFlag[sendIdx])
    {
        UART_DirectSend(uartSendBuf[sendIdx], DataProc_GetRadarDataLen());
        uartSendFlag[sendIdx] = 0;
        sendIdx = (sendIdx + 1) % SEND_BUF_CNT;
    }
    g_uartSendTime = 0;
}
#endif

void UART_Recv(void)
{
    g_cmdRecv.buf[g_cmdRecv.bufRecv][g_cmdRecv.curIndex++] = UART_ReceiveData(UART1);
    g_cmdRecv.idleCnt = 0;
    if (g_cmdRecv.curIndex >= CMD_RECV_BUF_SIZE)
    {
        g_cmdRecv.cmdReady = 1;
        g_cmdRecv.bufProc = g_cmdRecv.bufRecv;
        g_cmdRecv.bufRecv = (++g_cmdRecv.bufRecv) % BUF_MAX;
        g_cmdRecv.bufLen = g_cmdRecv.curIndex;
        g_cmdRecv.curIndex = 0;
    }
}

int fputc(int ch, FILE *f)
{
    UART_SendData(PRINT_UART, ch);
    while(!UART_GetFlagStatus(PRINT_UART, UART_FLAG_TXEPT));
    
    return ch;
}


