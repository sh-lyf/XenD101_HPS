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

#include "gd32f3x0.h"
#include "global_conf.h"

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

