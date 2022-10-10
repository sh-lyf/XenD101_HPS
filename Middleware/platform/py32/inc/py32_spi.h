/**
  ******************************************************************************
  * @file    py32_spi.h
  * @author  liujinyang@puyasemi.com
  * @brief   spi header file
  ******************************************************************************
  */
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f003_hal_conf.h"
#include "global_conf.h"

/* User can use this section to tailor SPIx instance used and associated
   resources */
/* Definition for SPIx clock resources */
#ifdef SPI_DATA_TEST

#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_NSS_PIN                     GPIO_PIN_15
#define SPIx_NSS_GPIO_PORT               GPIOA
#define SPIx_NSS_AF                      GPIO_AF0_SPI1
#define SPIx_SCK_PIN                     GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF0_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF0_SPI1

#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MISO_AF                     GPIO_AF0_SPI1
#else

#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_NSS_PIN                     GPIO_PIN_4
#define SPIx_NSS_GPIO_PORT               GPIOA
#define SPIx_NSS_AF                      GPIO_AF0_SPI1
#define SPIx_SCK_PIN                     GPIO_PIN_1
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_SCK_AF                      GPIO_AF0_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_2
#define SPIx_MOSI_GPIO_PORT              GPIOA
#define SPIx_MOSI_AF                     GPIO_AF0_SPI1

#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MISO_AF                     GPIO_AF0_SPI1

#endif
/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler

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

#ifdef SPI_DATA_TEST

#define DATA_RECV_BUF_SIZE          (SPI_FRAME_LEN_MAX) //(SPI_FRAME_LEN_MAX * 2) /*ping-pong buffer*/
#else
#define DATA_RECV_BUF_SIZE          (SPI_FRAME_LEN_MAX * 2) /*ping-pong buffer*/
#endif
extern SPI_HandleTypeDef SpiHandle;

extern uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE];
extern volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

void SPI_Init(uint16_t dmaBufLen);
void SPI_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif

