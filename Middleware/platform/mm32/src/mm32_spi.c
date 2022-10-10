/**
  ******************************************************************************
  * @file    gd32_spi.c
  * @author  ting.gao@iclegend.com
  * @brief   spi services
  ******************************************************************************
  */
#include "mm32_spi.h"
#include "utilities.h"

uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE];
volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

static void SPI1_NVIC_Init(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void SPI1_GPIO_SlaveInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1RSTR_SPI1, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = SPI_NSS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(SPI_NSS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(SPI_MOSI_PORT, SPI_MOSI_AFSOURCE, SPI_MOSI_AFMODE);
    GPIO_PinAFConfig(SPI_NSS_PORT, SPI_NSS_AFSOURCE, SPI_NSS_AFMODE);
    GPIO_PinAFConfig(SPI_SCK_PORT, SPI_SCK_AFSOURCE, SPI_SCK_AFMODE);
}

static void SPI1_SlaveConfig(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SPI1, ENABLE);
    SPI_StructInit(&SPI_InitStructure);
    
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_DataWidth = 8;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    HAL_SPI_Init(SPI, &SPI_InitStructure);

    SPI_ITConfig(SPI, SPI_IT_RX, ENABLE);
    SPI_BiDirectionalLineConfig(SPI, SPI_Direction_Rx);
    //SPI_BiDirectionalLineConfig(SPI, SPI_Direction_Tx);
    SPI_Cmd(SPI, ENABLE);
}

void SPI_Init(uint16_t dmaBufLen)
{
    SPI1_NVIC_Init();
    SPI1_GPIO_SlaveInit();
    SPI1_SlaveConfig();
}

void SPI_DeInit(void)
{
    HAL_SPI_DeInit(SPI);
}


