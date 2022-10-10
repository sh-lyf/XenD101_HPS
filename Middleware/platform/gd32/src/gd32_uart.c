/**
  ******************************************************************************
  * @file    gd32_uart.c
  * @author  ting.gao@iclegend.com
  * @brief   uart services
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "gd32_uart.h"
#include "gd32_spi.h"


CMD_RECV_T g_cmdRecv;
volatile uint8_t g_uartDmaSendingFlag = 0;
#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
static uint8_t uartSendBuf[SEND_BUF_CNT][SPI_FRAME_LEN_MAX];
static volatile uint8_t uartSendFlag[SEND_BUF_CNT];
#endif
static void uart0_dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    
    RCU_APB2EN |=RCU_APB2EN_CFGCMPEN;
    SYSCFG_CFG0 |=SYSCFG_CFG0_USART0_TX_DMA_RMP; /*uart0 tx use dma channel 3*/

    rcu_periph_clock_enable(RCU_DMA);
    dma_deinit(USART0_DMA_CHANEL);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_addr = (uint32_t )&USART_TDATA(USART0);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(USART0_DMA_CHANEL,&dma_init_struct);

    dma_circulation_disable(USART0_DMA_CHANEL);
    dma_memory_to_memory_disable(USART0_DMA_CHANEL);

    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    
    nvic_irq_enable(DMA_Channel3_4_IRQn, 0, 0);
	dma_interrupt_enable(USART0_DMA_CHANEL, DMA_INT_FTF);
}

static void uart0_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9 | GPIO_PIN_10);
}

#ifdef CONFIG_SUPPORT_UART1_DEBUG
static void uart1_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2 | GPIO_PIN_3);
}
#endif

static void uart0_config(void)
{
    rcu_periph_clock_enable(RCU_USART0);
    
    usart_deinit(USART0);
    usart_baudrate_set(USART0, USART0_BAUDRATE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

#ifdef CONFIG_SUPPORT_UART1_DEBUG
static void uart1_config(void)
{
    rcu_periph_clock_enable(RCU_USART1);
    
    usart_deinit(USART1);
    usart_baudrate_set(USART1, USART1_BAUDRATE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
}
#endif

void UART_Init(void)
{
    uart0_gpio_config();
    uart0_config();
    uart0_dma_config();
    usart_enable(USART0);
    
#ifdef CONFIG_SUPPORT_UART1_DEBUG
    uart1_gpio_config();
    uart1_config();
    usart_enable(USART1);
#endif
}

void UART0_DmaSend(uint8_t* buf, uint16_t size)
{    
    dma_channel_disable(USART0_DMA_CHANEL);
    dma_memory_address_config(USART0_DMA_CHANEL, (uint32_t)buf);
    dma_transfer_number_config(USART0_DMA_CHANEL, size);
    dma_channel_enable(USART0_DMA_CHANEL);
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

#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
void UART_Send2Buf(uint8_t* pBuffer, uint16_t uBufLen)
{	
    static uint16_t dataIndex = 0;
    static uint8_t bufIndex = 0;
    uint16_t availableBufLen = 0;
    uint16_t copyDataLen = 0;
    uint16_t sendBufLen = SPI_FRAME_LEN_MAX;
    
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
    
    if (g_uartDmaSendingFlag)
    {
        return;
    }

    if (uartSendFlag[sendIdx])
    {
        UART0_DmaSend(uartSendBuf[sendIdx], SPI_FRAME_LEN_MAX);
        uartSendFlag[sendIdx] = 0;
        sendIdx = (sendIdx + 1) % SEND_BUF_CNT;
        g_uartDmaSendingFlag = 1;
    }
}
#endif

void UART_Recv(void)
{
    g_cmdRecv.buf[g_cmdRecv.bufRecv][g_cmdRecv.curIndex++] = usart_data_receive(USART0);
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
    uint32_t usart_periph = 0;
    
#ifdef CONFIG_SUPPORT_UART1_DEBUG
    usart_periph = USART1;
#else
    usart_periph = USART0;
#endif
    usart_data_transmit(usart_periph, (uint8_t)ch);
    while(RESET == usart_flag_get(usart_periph, USART_FLAG_TBE));
    return ch;
}


