/**
  ******************************************************************************
  * @file    stm32_spi.c
  * @author  iclm team
  * @brief   spi services
  ******************************************************************************
  */
#include "stm32_spi.h"
#include "utilities.h"
#include "dataprocess.h"
#include "rtos.h"

#ifdef SUPPORT_2TX_4RX_BOARD
uint8_t g_dataRecvBuf[CHANNEL_MAX-1][DATA_RECV_BUF_SIZE] __ALIGN(4);
uint8_t g_dataRecvBufSram4[DATA_RECV_BUF_SIZE] __ALIGN(4) __SRAM4_DATA;
#else
uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE] __ALIGN(4);
#endif
volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

static SPI_HandleTypeDef hspi1;
static SPI_HandleTypeDef hspi2;
static SPI_HandleTypeDef hspi4;
static SPI_HandleTypeDef hspi6;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi4_rx;
DMA_HandleTypeDef hdma_spi6_rx;

static uint16_t curSpiFrameLen = 0;

#ifndef SUPPORT_TSPI_IO
static void SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_SLAVE;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}

static void SPI1_DeInit(void)
{
    if (HAL_SPI_DeInit(&hspi1) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}
#endif

#if defined(STM32H750xx)
static void SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_SLAVE;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}

static void SPI2_DeInit(void)
{
    if (HAL_SPI_DeInit(&hspi2) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}
#endif

static void SPI4_Init(void)
{
    hspi4.Instance = SPI4;
    hspi4.Init.Mode = SPI_MODE_SLAVE;
    hspi4.Init.Direction = SPI_DIRECTION_2LINES;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi4.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi4.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi4) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}

static void SPI4_DeInit(void)
{
    if (HAL_SPI_DeInit(&hspi4) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}

#if defined(STM32H750xx)
static void SPI6_Init(void)
{
    hspi6.Instance = SPI6;
    hspi6.Init.Mode = SPI_MODE_SLAVE;
    hspi6.Init.Direction = SPI_DIRECTION_2LINES;
    hspi6.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi6.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi6.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi6.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi6.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi6.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi6.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi6.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi6) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}

static void SPI6_DeInit(void)
{
    if (HAL_SPI_DeInit(&hspi6) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}
#endif

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(spiHandle->Instance==SPI1)
    {
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**SPI1 GPIO Configuration    
        PA4     ------> SPI1_NSS
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        __HAL_RCC_DMA2_CLK_ENABLE();
        hdma_spi1_rx.Instance = DMA2_Stream0;
#if defined(STM32F429xx)
        hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
#elif defined(STM32H750xx)
        hdma_spi1_rx.Init.Request = DMA_REQUEST_SPI1_RX;
#endif
        hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi1_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi1_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);
        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    }
    else if(spiHandle->Instance==SPI4)
    {
        __HAL_RCC_SPI4_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**SPI4 GPIO Configuration    
        PE11     ------> SPI4_NSS
        PE12     ------> SPI4_SCK
        PE13     ------> SPI4_MISO
        PE14     ------> SPI4_MOSI 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        __HAL_RCC_DMA2_CLK_ENABLE();        
        hdma_spi4_rx.Instance = DMA2_Stream3;
#if defined(STM32F429xx)
        hdma_spi4_rx.Init.Channel = DMA_CHANNEL_5;
#elif defined(STM32H750xx)
        hdma_spi4_rx.Init.Request = DMA_REQUEST_SPI4_RX;
#endif
        hdma_spi4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi4_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi4_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi4_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi4_rx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi4_rx);
        HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    }
#if defined(STM32H750xx)	
	else if(spiHandle->Instance==SPI2)
    {
        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI2 GPIO Configuration    
        PB12     ------> SPI2_NSS
        PB13     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        __HAL_RCC_DMA2_CLK_ENABLE();
        hdma_spi2_rx.Instance = DMA2_Stream1;
#if defined(STM32F429xx)
        hdma_spi2_rx.Init.Channel = DMA_CHANNEL_3;
#elif defined(STM32H750xx)
        hdma_spi2_rx.Init.Request = DMA_REQUEST_SPI2_RX;
#endif
        hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi2_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi2_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi2_rx);
        HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    }
	else if(spiHandle->Instance==SPI6)
    {
        __HAL_RCC_SPI6_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI1 GPIO Configuration    
        PA15     ------> SPI6_NSS
        PB3     ------> SPI6_SCK
        PB4     ------> SPI6_MISO
        PB5     ------> SPI6_MOSI 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_SPI6;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_SPI6;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        __HAL_RCC_BDMA_CLK_ENABLE();
        hdma_spi6_rx.Instance = BDMA_Channel0;
#if defined(STM32F429xx)
        hdma_spi6_rx.Init.Channel = DMA_CHANNEL_3;
#elif defined(STM32H750xx)
        hdma_spi6_rx.Init.Request = BDMA_REQUEST_SPI6_RX;
#endif
        hdma_spi6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi6_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi6_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi6_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_spi6_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_spi6_rx.Init.MemBurst = DMA_MBURST_SINGLE;       
		hdma_spi6_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;  
        if (HAL_DMA_Init(&hdma_spi6_rx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi6_rx);
        HAL_NVIC_SetPriority(BDMA_Channel0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(BDMA_Channel0_IRQn);        
    }
#endif
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
    if(spiHandle->Instance==SPI1)
    {
        __HAL_RCC_SPI1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }
    }
	else if(spiHandle->Instance==SPI2)
    {
        __HAL_RCC_SPI2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }
    }
    else if(spiHandle->Instance==SPI4)
    {
        __HAL_RCC_SPI4_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14);
        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
        {

            RunFailed((uint8_t *)__FILE__, __LINE__);
        }
    }
	else if(spiHandle->Instance==SPI6)
    {
        __HAL_RCC_SPI6_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }
    }
} 

#ifndef SUPPORT_TSPI_IO
static void SPI1_DMA_XferCplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;
    
    if (g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_1])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_0] = 1;
    
	radarData.buf = g_dataRecvBuf[CHANNEL_1];
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_0;
    
    DataProc_Send2RadarDataQueue(CHANNEL_1, &radarData);
}

static void SPI1_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_0])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_1] = 1;

	radarData.buf = &(g_dataRecvBuf[CHANNEL_1][curSpiFrameLen]);
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_1;
    
    DataProc_Send2RadarDataQueue(CHANNEL_1, &radarData); 
}

static void SPI1_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
    {
		hspi->State = HAL_SPI_STATE_READY;
	}
	
	HAL_SPI_ErrorCallback(hspi);
} 
#endif

#if defined(STM32H750xx)
static void SPI2_DMA_XferCplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;
    
    if (g_dataRecvFlag[CHANNEL_2][DMA_RECV_FLAG_MEM_1])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_2][DMA_RECV_FLAG_MEM_0] = 1;
    
	radarData.buf = g_dataRecvBuf[CHANNEL_2];
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_0;
    
    DataProc_Send2RadarDataQueue(CHANNEL_2, &radarData);
}

static void SPI2_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_2][DMA_RECV_FLAG_MEM_0])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_2][DMA_RECV_FLAG_MEM_1] = 1;

	radarData.buf = &(g_dataRecvBuf[CHANNEL_2][curSpiFrameLen]);
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_1;
    
    DataProc_Send2RadarDataQueue(CHANNEL_2, &radarData); 
}

static void SPI2_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
    {
		hspi->State = HAL_SPI_STATE_READY;
	}
	
	HAL_SPI_ErrorCallback(hspi);
} 
#endif
static void SPI4_DMA_XferCplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0] = 1;

	radarData.buf = g_dataRecvBuf[CHANNEL_0];
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_0;
        
    DataProc_Send2RadarDataQueue(CHANNEL_0, &radarData); 
}

static void SPI4_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1] = 1;

	radarData.buf = &(g_dataRecvBuf[CHANNEL_0][curSpiFrameLen]);
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_1;

    DataProc_Send2RadarDataQueue(CHANNEL_0, &radarData); 
}

static void SPI4_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
    {
		hspi->State = HAL_SPI_STATE_READY;
	}
	
	HAL_SPI_ErrorCallback(hspi);
} 

#if defined(STM32H750xx)
static void SPI6_DMA_XferCplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;
    
    if (g_dataRecvFlag[CHANNEL_3][DMA_RECV_FLAG_MEM_1])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_3][DMA_RECV_FLAG_MEM_0] = 1;
    
	radarData.buf = g_dataRecvBufSram4;
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_0;
    
    DataProc_Send2RadarDataQueue(CHANNEL_3, &radarData);
}

static void SPI6_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
{
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_3][DMA_RECV_FLAG_MEM_0])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_3][DMA_RECV_FLAG_MEM_1] = 1;

	radarData.buf = &g_dataRecvBufSram4[curSpiFrameLen];
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_1;
    
    DataProc_Send2RadarDataQueue(CHANNEL_3, &radarData); 
}

static void SPI6_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
    {
		hspi->State = HAL_SPI_STATE_READY;
	}
	
	HAL_SPI_ErrorCallback(hspi);
} 
#endif

static void SPI_DMA_Enable(uint16_t bufSize)
{
    uint32_t srcAddr = 0;
    
    if (bufSize > (DATA_RECV_BUF_SIZE >> 1))
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

#ifndef SUPPORT_TSPI_IO
    hspi1.hdmarx->XferCpltCallback = SPI1_DMA_XferCplt;
    hspi1.hdmarx->XferM1CpltCallback = SPI1_DMA_XferM1Cplt;
    hspi1.hdmarx->XferErrorCallback = SPI1_DMA_ErrorCallback;

#if defined(STM32F429xx)
    srcAddr = (uint32_t)&(hspi1.Instance->DR);
#elif defined(STM32H750xx)
    srcAddr = (uint32_t)&(hspi1.Instance->RXDR);
#endif
    __HAL_UNLOCK(hspi1.hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(hspi1.hdmarx, srcAddr, 
        (uint32_t)g_dataRecvBuf[CHANNEL_1], (uint32_t)&(g_dataRecvBuf[CHANNEL_1][bufSize]),
        (uint32_t)bufSize) != HAL_OK) 
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    if((hspi1.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        __HAL_SPI_ENABLE(&hspi1);
    }

#if defined(STM32F429xx)
    SET_BIT(hspi1.Instance->CR2, SPI_CR2_ERRIE);
    SET_BIT(hspi1.Instance->CR2, SPI_CR2_RXDMAEN); 
#elif defined(STM32H750xx)
    SET_BIT(hspi1.Instance->IER, (SPI_IT_OVR | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));
    SET_BIT(hspi1.Instance->CFG1, SPI_CFG1_RXDMAEN);
#endif
#endif
        
	hspi4.hdmarx->XferCpltCallback = SPI4_DMA_XferCplt;
    hspi4.hdmarx->XferM1CpltCallback = SPI4_DMA_XferM1Cplt;
    hspi4.hdmarx->XferErrorCallback = SPI4_DMA_ErrorCallback;

#if defined(STM32F429xx)
    srcAddr = (uint32_t)&(hspi4.Instance->DR);
#elif defined(STM32H750xx)
    srcAddr = (uint32_t)&(hspi4.Instance->RXDR);
#endif
    __HAL_UNLOCK(hspi4.hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(hspi4.hdmarx, srcAddr, 
        (uint32_t)g_dataRecvBuf[CHANNEL_0], (uint32_t)&(g_dataRecvBuf[CHANNEL_0][bufSize]),
        (uint32_t)bufSize) != HAL_OK) 
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    if((hspi4.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        __HAL_SPI_ENABLE(&hspi4);
    }
 
#if defined(STM32F429xx)
    SET_BIT(hspi4.Instance->CR2, SPI_CR2_ERRIE);
    SET_BIT(hspi4.Instance->CR2, SPI_CR2_RXDMAEN); 
#elif defined(STM32H750xx)
    SET_BIT(hspi4.Instance->IER, (SPI_IT_OVR | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));
    SET_BIT(hspi4.Instance->CFG1, SPI_CFG1_RXDMAEN);
#endif
	
#if defined(STM32H750xx)	
	hspi2.hdmarx->XferCpltCallback = SPI2_DMA_XferCplt;
    hspi2.hdmarx->XferM1CpltCallback = SPI2_DMA_XferM1Cplt;
    hspi2.hdmarx->XferErrorCallback = SPI2_DMA_ErrorCallback;

#if defined(STM32F429xx)
    srcAddr = (uint32_t)&(hspi2.Instance->DR);
#elif defined(STM32H750xx)
    srcAddr = (uint32_t)&(hspi2.Instance->RXDR);
#endif
    __HAL_UNLOCK(hspi2.hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(hspi2.hdmarx, srcAddr, 
        (uint32_t)g_dataRecvBuf[CHANNEL_2], (uint32_t)&(g_dataRecvBuf[CHANNEL_2][bufSize]),
        (uint32_t)bufSize) != HAL_OK) 
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    if((hspi2.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        __HAL_SPI_ENABLE(&hspi2);
    }
	
#if defined(STM32F429xx)
    SET_BIT(hspi2.Instance->CR2, SPI_CR2_ERRIE);
    SET_BIT(hspi2.Instance->CR2, SPI_CR2_RXDMAEN); 
#elif defined(STM32H750xx)
    SET_BIT(hspi2.Instance->IER, (SPI_IT_OVR | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));
    SET_BIT(hspi2.Instance->CFG1, SPI_CFG1_RXDMAEN);
#endif
	
	hspi6.hdmarx->XferCpltCallback = SPI6_DMA_XferCplt;
    hspi6.hdmarx->XferM1CpltCallback = SPI6_DMA_XferM1Cplt;
    hspi6.hdmarx->XferErrorCallback = SPI6_DMA_ErrorCallback;

#if defined(STM32F429xx)
    srcAddr = (uint32_t)&(hspi6.Instance->DR);
#elif defined(STM32H750xx)
    srcAddr = (uint32_t)&(hspi6.Instance->RXDR);
#endif
    __HAL_UNLOCK(hspi6.hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(hspi6.hdmarx, srcAddr, 
        (uint32_t)g_dataRecvBufSram4, (uint32_t)&g_dataRecvBufSram4[bufSize],
        (uint32_t)bufSize) != HAL_OK) 
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    if((hspi6.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        __HAL_SPI_ENABLE(&hspi6);
    }

#if defined(STM32F429xx)
    SET_BIT(hspi6.Instance->CR2, SPI_CR2_ERRIE);
    SET_BIT(hspi6.Instance->CR2, SPI_CR2_RXDMAEN); 
#elif defined(STM32H750xx)
    SET_BIT(hspi6.Instance->IER, (SPI_IT_OVR | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));
    SET_BIT(hspi6.Instance->CFG1, SPI_CFG1_RXDMAEN);
#endif

#endif
}

void SPI_Init(uint16_t dmaBufLen)
{ 
#ifndef SUPPORT_TSPI_IO
    SPI1_Init(); /*LNA1-TSPI_DA1*/
#endif
    SPI4_Init(); /*LNA2-TSPI_DA0*/	
#if defined(STM32H750xx)   
	SPI2_Init();
	SPI6_Init();	
#endif	
    curSpiFrameLen = dmaBufLen >> 1;
    SPI_DMA_Enable(curSpiFrameLen);
}

void SPI_DeInit(void)
{
#ifndef SUPPORT_TSPI_IO
    SPI1_DeInit(); 
#endif
    SPI4_DeInit(); 
#if defined(STM32H750xx)   
	SPI2_DeInit(); 
	SPI6_DeInit(); 	
#endif
}

