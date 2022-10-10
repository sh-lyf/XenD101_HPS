/**
  ******************************************************************************
  * @file    gd32_spi.c
  * @author  ting.gao@iclegend.com
  * @brief   spi services
  ******************************************************************************
  */
#include "gd32_spi.h"
#include "utilities.h"

uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE];
volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

static void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_DMA);
    rcu_periph_clock_enable(RCU_SPI0);
}

void gpio_config_tspi1(void) /*LNA2-TSPI_DA1*/
{
    /*
        PA15 --- SPI0 NSS
        PB3  --- SPI0 SCK
        PB5  --- SPI0 MOSI
    */
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_15);
	gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_3 |GPIO_PIN_5);
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 |GPIO_PIN_5);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_15);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_3 |GPIO_PIN_5);
}

static void gpio_config_tspi0(void) /*LNA1-TSPI_DA0*/
{
    /*
        PA4  --- SPI0 NSS
        PA5  --- SPI0 SCK
        PA7  --- SPI0 MOSI
    */
	gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
}

static void gpio_deconfig(void)
{
	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_15);
	gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3 |GPIO_PIN_5);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_15);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_3 |GPIO_PIN_5);
}

static void dma_config(uint16_t dmaBufLen)
{
    dma_parameter_struct  dma_init_struct;
   
    /* SPI0 receive dma config */
    dma_struct_para_init(&dma_init_struct);
	dma_deinit(DMA_CH1);
    dma_init_struct.periph_addr = (uint32_t )&SPI_DATA(SPI0);
    dma_init_struct.memory_addr = (uint32_t)g_dataRecvBuf[CHANNEL_0];
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.number = dmaBufLen;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;	
	dma_init(DMA_CH1, &dma_init_struct);
	dma_circulation_enable(DMA_CH1);
}	

static void spi_config(void)
{
    spi_parameter_struct spi_init_struct;

    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);
	
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_SLAVE;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_HARD;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);	
}


void SPI_Init(uint16_t dmaBufLen)
{
    rcu_config();
    gpio_config_tspi0();
    dma_config(dmaBufLen);
    spi_config();
    spi_enable(SPI0);	
	
    dma_channel_enable(DMA_CH1);	
    spi_dma_enable(SPI0, SPI_DMA_RECEIVE);	
	
	nvic_irq_enable(DMA_Channel1_2_IRQn, 0, 0);
	dma_interrupt_enable(DMA_CH1, DMA_INT_FTF | DMA_INT_HTF);	
}

void SPI_DeInit(void)
{
    spi_disable(SPI0);
    gpio_deconfig();
    dma_deinit(DMA_CH1);
}


