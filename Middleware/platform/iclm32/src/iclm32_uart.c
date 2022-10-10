/**
  ******************************************************************************
  * @file    iclm32_uart.c
  * @author  ting.gao@iclegend.com
  * @brief   uart services
  ******************************************************************************
  */
#include <stdio.h>
#include <stdarg.h>
#include "iclm32_uart.h"

CMD_RECV_T g_cmdRecv;

void UART_Init(void)
{   
}

void UART_RecvTimeout(void)
{

}

void UART_Recv(void)
{

}

int uart_put(int ch)
{
#ifdef TEAK
#else
    while (UART0_REG(UART_REG_TXFIFO) & UART_TXFULL);
    UART0_REG(UART_REG_TXFIFO) = ch;
#endif
    return ch;
}

void uart_printf(const char *fmt, ...)
{
    char buf[80], *p;
    va_list ap;
    
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    for (p = buf; *p; ++p)
    {
        uart_put(*p);
    }
    va_end(ap);
}

