/**
  ******************************************************************************
  * @file    py32_spi.c
  * @author  liujinyang@puyasemi.com
  * @brief   spi services
  ******************************************************************************
  */
#include "platform.h"
#include "utilities.h"
#include "dataprocess.h"

SPI_HandleTypeDef SpiHandle;
DMA_HandleTypeDef HdmaCh2;

#ifdef SPI_DATA_TEST
uint8_t g_DestData[DATA_RECV_BUF_SIZE];
#endif
uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE];
volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];
/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	if(hspi->Instance == SPIx)
	{     
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		SPIx_SCK_GPIO_CLK_ENABLE();
		SPIx_MISO_GPIO_CLK_ENABLE();
		SPIx_MOSI_GPIO_CLK_ENABLE();
		SPIx_NSS_GPIO_CLK_ENABLE();
		/*
			PA4-NSS	(AF0)
			PA5-SCK(AF0)
			PA6-MISO(AF0)
			PA7-MOSI(AF0)
		*/
		/* Enable SPI clock */
		SPIx_CLK_ENABLE(); 

		/*##-2- Configure peripheral GPIO ##########################################*/  
		/* SPI SCK GPIO pin configuration  */
		GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = SPIx_SCK_AF;
		HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

		/* SPI MISO GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MISO_PIN;
		GPIO_InitStruct.Alternate = SPIx_MISO_AF;
		HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

		/* SPI MOSI GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
		GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
		HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
		
		/* SPI NSS GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_NSS_PIN;
		GPIO_InitStruct.Alternate = SPIx_NSS_AF;
		HAL_GPIO_Init(SPIx_NSS_GPIO_PORT, &GPIO_InitStruct);
		
//		HAL_NVIC_SetPriority(SPIx_IRQn, 1, 0);
//		HAL_NVIC_EnableIRQ(SPIx_IRQn);
		__HAL_RCC_DMA_CLK_ENABLE();
		__HAL_RCC_SYSCFG_CLK_ENABLE();

		/* Configure the DMA handler for Transmission process */
		HdmaCh2.Instance                 = DMA1_Channel2;

		HdmaCh2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
		HdmaCh2.Init.PeriphInc           = DMA_PINC_DISABLE;
		HdmaCh2.Init.MemInc              = DMA_MINC_ENABLE;
		if(hspi->Init.DataSize<=SPI_DATASIZE_8BIT)
		{
			HdmaCh2.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
			HdmaCh2.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		}
		else
		{
			HdmaCh2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			HdmaCh2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
		}
		HdmaCh2.Init.Mode                = DMA_NORMAL;
		HdmaCh2.Init.Priority            = DMA_PRIORITY_HIGH;

		HAL_DMA_Init(&HdmaCh2);

		DMA1_Channel2->CCR&=~(3<<12);
		DMA1_Channel2->CCR|=(3<<12);
		//    /* Associate the initialized DMA handle to the the SPI handle */
		__HAL_LINKDMA(hspi, hdmarx, HdmaCh2);

		HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1, 1);
		HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	}
}

/**
  * @brief SPI MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to its default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPIx)
	{   
		/*##-1- Reset peripherals ##################################################*/
		SPIx_FORCE_RESET();
		SPIx_RELEASE_RESET();

		/*##-2- Disable peripherals and GPIO Clocks ################################*/
		/* Configure SPI SCK as alternate function  */
		HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
		/* Configure SPI MISO as alternate function  */
		HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
		/* Configure SPI MOSI as alternate function  */
		HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
	}
}

void SPI_Init(uint16_t dmaBufLen)
{
	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	SpiHandle.Instance               = SPIx;

	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.NSS               = SPI_NSS_HARD_INPUT;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
	SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;

	SpiHandle.Init.Mode = SPI_MODE_SLAVE;

	if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	{
		/* Initialization Error */
		while(1);
	}
	//fast mode
//	SPIx->CR2 |= 0x1<<15;
	HAL_SYSCFG_DMA_Req(0x200);   

	HAL_SPI_Receive_DMA(&SpiHandle, (uint8_t *)g_dataRecvBuf, RadarPara.dataLen);
	
	#ifdef SPI_DATA_TEST
	for(uint8_t i=0; i< SPI_FRAME_LEN_MAX; i++) 
	{
		g_DestData[i] = i+1;
	}
	#endif
}

void SPI_DeInit(void)
{
	HAL_SPI_DeInit(&SpiHandle);
}


uint8_t CheckData(uint8_t *src, uint8_t *dst,uint32_t size)
{
	uint8_t i;

	for(i = 0; i < size; i++)
	{
		if(src[i] != dst[i])
			return 1;
	}
	return 0;
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	
	#ifdef SPI_DATA_TEST
	static 	uint32_t transfer_cnt = 0;

	//if(transfer_cnt++ < 10)
//	{
//		if(CheckData((uint8_t*)g_dataRecvBuf,(uint8_t *)g_DestData,SPI_FRAME_LEN_MAX))
//		{
//			printf("error! ");
//		}
//		else
//		{
//			HAL_SPI_Receive_DMA(&SpiHandle, (uint8_t *) g_dataRecvBuf, SPI_FRAME_LEN_MAX);

//			printf("pass! ");

//		}

//	}
    static uint16_t cnt = 0;
	
	cnt += SPI_FRAME_LEN_MAX;

	if (cnt >= (SPI_FRAME_LEN_MAX << 1))
	{
		HAL_SPI_Receive_DMA(&SpiHandle, (uint8_t *) g_dataRecvBuf, SPI_FRAME_LEN_MAX);
		g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1] = 1;
		cnt = 0;
	}
	else if (cnt == SPI_FRAME_LEN_MAX)
	{
		HAL_SPI_Receive_DMA(&SpiHandle, (uint8_t *) g_dataRecvBuf + SPI_FRAME_LEN_MAX, SPI_FRAME_LEN_MAX);

		g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0] = 1;
	}
	
	#else
    static uint16_t cnt = 0;
	
	cnt += RadarPara.dataLen;

	if (cnt >= (RadarPara.dataLen << 1))
	{
		HAL_SPI_Receive_DMA(&SpiHandle, (uint8_t *) g_dataRecvBuf, RadarPara.dataLen);
		g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1] = 1;
		cnt = 0;
	}
	else if (cnt == RadarPara.dataLen)
	{
		HAL_SPI_Receive_DMA(&SpiHandle, (uint8_t *) g_dataRecvBuf + RadarPara.dataLen, RadarPara.dataLen);

		g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0] = 1;
	}
	#endif

	//printf("Recv Data: cnt = %d\r\n",cnt);
	//for(int i = 0; i < SPI_FRAME_LEN_MAX; i++)
	//{
	//	printf("%x ",g_dataRecvBuf[CHANNEL_0][i]);
	//}
	//printf("\r\n");


}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	printf("hspi->ErrorCode = %d ",hspi->ErrorCode);
}
