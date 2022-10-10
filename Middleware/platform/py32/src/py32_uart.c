/**
  ******************************************************************************
  * @file    py32_uart.c
  * @author  liujinyang@puyasemi.com
  * @brief   uart services
  ******************************************************************************
  */
#include "platform.h"
#include "dataprocess.h"

UART_HandleTypeDef UartHandle;

CMD_RECV_T g_cmdRecv;
#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
static uint8_t uartSendBuf[SEND_BUF_CNT][SPI_FRAME_LEN_MAX];
static volatile uint8_t uartSendFlag[SEND_BUF_CNT];
uint8_t g_uartSendTime = 0;
#endif

void UART1_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	USARTx_TX_GPIO_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();


	/* Enable USARTx clock */
	USARTx_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = USARTx_TX_AF;

	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = USARTx_RX_PIN;
	GPIO_InitStruct.Alternate = USARTx_RX_AF;

	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

	/*##-3- Configure the NVIC for UART ########################################*/
	/* NVIC for USART */
	HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USARTx_IRQn);

}

#ifdef PRINT_BY_UART2
void UART2_GPIO_Init(void)
{
}
#endif

void UART_Init(void)
{
	UART1_GPIO_Init();
	
    UartHandle.Instance        = USARTx;

    UartHandle.Init.BaudRate   = UART_BAUDRATE;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;

    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;

    HAL_UART_Init(&UartHandle);

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
}



int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;

}


