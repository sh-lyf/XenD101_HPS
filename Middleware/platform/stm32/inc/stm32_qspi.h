/**
  ******************************************************************************
  * @file    stm32_qspi.h
  * @author  iclm team
  * @brief   qspi header file
  ******************************************************************************
  */
  
#ifndef __STM32_QSPI_H__
#define __STM32_QSPI_H__

#include "stm32h7xx_hal.h"

#define QSPI_MEM_START            (0x90000000U)

#define QSPI_FORCE_RESET()         __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()       __HAL_RCC_QSPI_RELEASE_RESET()
#define QSPI_CLK_DISABLE()         __HAL_RCC_QSPI_CLK_DISABLE()

uint8_t QSPI_Init(void);												//初始化QSPI
void QSPI_DeInit(void);
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle);			//QSPI发送命令
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen);							//QSPI接收数据
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen);							//QSPI发送数据
uint32_t QSPI_EnableMemoryMappedMode(void);
HAL_StatusTypeDef QSPI_Command(QSPI_CommandTypeDef *cmd, uint32_t Timeout);

#endif

