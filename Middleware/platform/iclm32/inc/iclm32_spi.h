/**
  ******************************************************************************
  * @file    iclm32_spi.h
  * @author  ting.gao@iclegend.com
  * @brief   spi header file
  ******************************************************************************
  */
#ifndef __ICLM32_SPI_H__
#define __ICLM32_SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif 

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


#ifdef __cplusplus
}
#endif

#endif

