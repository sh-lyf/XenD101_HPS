/**
  ******************************************************************************
  * @file    gd32_spi.h
  * @author  ting.gao@iclegend.com
  * @brief   spi header file
  ******************************************************************************
  */
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "hal_conf.h"
#include "global_conf.h"

#define SPI  SPI1

#define SPI_MOSI_BUSCLK                  RCC_AHBENR_GPIOA
#define SPI_MOSI_PIN                     GPIO_Pin_9
#define SPI_MOSI_PORT                    GPIOA
#define SPI_MOSI_AFSOURCE                GPIO_PinSource9
#define SPI_MOSI_AFMODE                  GPIO_AF_0

#define SPI_NSS_BUSCLK                  RCC_AHBENR_GPIOA
#define SPI_NSS_PIN                     GPIO_Pin_15
#define SPI_NSS_PORT                    GPIOA
#define SPI_NSS_AFSOURCE                GPIO_PinSource15
#define SPI_NSS_AFMODE                  GPIO_AF_0

#define SPI_SCK_BUSCLK                  RCC_AHBENR_GPIOA
#define SPI_SCK_PIN                     GPIO_Pin_8
#define SPI_SCK_PORT                    GPIOA
#define SPI_SCK_AFSOURCE                GPIO_PinSource8
#define SPI_SCK_AFMODE                  GPIO_AF_0

typedef enum
{
    DMA_RECV_FLAG_MEM_0        = 0,
    DMA_RECV_FLAG_MEM_1        = 1,
    DMA_RECV_FLAG_MAX
}dmaRecvFlagEnum;

#define SPI_FRAME_DLEN_MAX      RADAR_DATA_MAX_LEN
#define SPI_FRAME_HLEN          (4)
#define SPI_FRAME_TLEN          (4)
#define SPI_FRAME_LEN_MAX       (SPI_FRAME_DLEN_MAX + SPI_FRAME_HLEN + SPI_FRAME_TLEN)

#define DATA_RECV_BUF_SIZE           (SPI_FRAME_LEN_MAX * 2) /*ping-pong buffer*/

extern uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE];
extern volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

void SPI_Init(uint16_t dmaBufLen);
void SPI_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif

