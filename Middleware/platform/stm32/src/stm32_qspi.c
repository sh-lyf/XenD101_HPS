/**
  ******************************************************************************
  * @file    stm32_qspi.c
  * @author  iclm team
  * @brief   qspi services
  ******************************************************************************
  */
#include "w25qxx.h"
#include "stm32_qspi.h"

static QSPI_HandleTypeDef QSPI_Handler;    //QSPI句柄

//等待状态标志
//flag:需要等待的标志位
//sta:需要等待的状态
//wtime:等待时间
//返回值:0,等待成功.
//	     1,等待失败.
uint8_t QSPI_Wait_Flag(uint32_t flag,uint8_t sta,uint32_t wtime)
{
	uint8_t flagsta=0;
	while(wtime)
	{
		flagsta=(QUADSPI->SR&flag)?1:0; 
		if(flagsta==sta)break;
		wtime--;
	}
	if(wtime)return 0;
	else return 1;
}

//QSPI初始化
uint8_t QSPI_Init(void)
{
    QSPI_Handler.Instance=QUADSPI;                          //QSPI
    QSPI_Handler.Init.ClockPrescaler=1;                     //QPSI分频比，W25Q256最大频率为104M，
                                                            //所以此处应该为2，QSPI频率就为200/(1+1)=100MHZ
    QSPI_Handler.Init.FifoThreshold=4;                      //FIFO阈值为4个字节
    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;//采样移位半个周期(DDR模式下,必须设置为0)
    QSPI_Handler.Init.FlashSize=POSITION_VAL(0X800000)-1;  //SPI FLASH大小，W25Q64大小为8M字节
    QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;//片选高电平时间为5个时钟(10*5=55ns),即手册里面的tSHSL参数
    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          //模式0
    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_1;              //第一片flash
    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE;     //禁止双闪存模式
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) return 0;      //QSPI初始化成功
    else return 1;
}

//QSPI底层驱动,引脚配置，时钟使能
//此函数会被HAL_QSPI_Init()调用
//hqspi:QSPI句柄
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_QSPI_CLK_ENABLE();        //使能QSPI时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOB时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();       //使能GPIOD时钟
	__HAL_RCC_GPIOE_CLK_ENABLE();       //使能GPIOD时钟
    
    //初始化PB10 片选信号
    GPIO_Initure.Pin=GPIO_PIN_10;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //复用
    GPIO_Initure.Pull=GPIO_PULLUP;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;   //复用为QSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //PD11,12
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
    //PB2
    GPIO_Initure.Pin=GPIO_PIN_2;
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;   //复用为QSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //PD13
    GPIO_Initure.Pin=GPIO_PIN_13;
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
		
	//PE2
    GPIO_Initure.Pin=GPIO_PIN_2;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi)
{

  /*##-1- Disable peripherals ################################################*/
  /* De-Configure QSPI pins */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11);
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);
  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

  /*##-3- Reset peripherals ##################################################*/
  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();

  /* Disable the QuadSPI memory interface clock */
  QSPI_CLK_DISABLE();
}

//QSPI发送命令
//cmd:要发送的指令
//addr:发送到的目的地址
//mode:模式,详细位定义如下:
//	mode[1:0]:指令模式: 00,无指令;01,单线传输指令;10,双线传输指令;11,四线传输指令.
//	mode[3:2]:地址模式: 00,无地址;01,单线传输地址;10,双线传输地址;11,四线传输地址.
//	mode[5:4]:地址长度: 00,8位地址;01,16位地址;10,24位地址;11,32位地址.
//	mode[7:6]:数据模式: 00,无数据;01,单线传输数据;10,双线传输数据;11,四线传输数据.
//dmcycle:空指令周期数
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle)
{
	QSPI_CommandTypeDef Cmdhandler;
    
	Cmdhandler.Instruction=cmd;									//指令
	Cmdhandler.Address=addr;									//地址
	Cmdhandler.DummyCycles=dmcycle;								//设置空指令周期数
	
	if(((mode>>0)&0x03) == 0)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_NONE;			//指令模式
	else if(((mode>>0)&0x03) == 1)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_1_LINE;			//指令模式
	else if(((mode>>0)&0x03) == 2)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_2_LINES;			//指令模式
	else if(((mode>>0)&0x03) == 3)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_4_LINES;			//指令模式
	
	if(((mode>>2)&0x03) == 0)
	Cmdhandler.AddressMode=QSPI_ADDRESS_NONE;   					//地址模式
	else if(((mode>>2)&0x03) == 1)
	Cmdhandler.AddressMode=QSPI_ADDRESS_1_LINE;   					//地址模式
	else if(((mode>>2)&0x03) == 2)
	Cmdhandler.AddressMode=QSPI_ADDRESS_2_LINES;   					//地址模式
	else if(((mode>>2)&0x03) == 3)
	Cmdhandler.AddressMode=QSPI_ADDRESS_4_LINES;   					//地址模式
	
	if(((mode>>4)&0x03) == 0)
	Cmdhandler.AddressSize=QSPI_ADDRESS_8_BITS;   					//地址长度
	else if(((mode>>4)&0x03) == 1)
	Cmdhandler.AddressSize=QSPI_ADDRESS_16_BITS;   					//地址长度
	else if(((mode>>4)&0x03) == 2)
	Cmdhandler.AddressSize=QSPI_ADDRESS_24_BITS;   					//地址长度
	else if(((mode>>4)&0x03) == 3)
	Cmdhandler.AddressSize=QSPI_ADDRESS_32_BITS;   					//地址长度
	
	if(((mode>>6)&0x03) == 0)
	Cmdhandler.DataMode=QSPI_DATA_NONE;             			//数据模式
	else if(((mode>>6)&0x03) == 1)
	Cmdhandler.DataMode=QSPI_DATA_1_LINE;             			//数据模式
	else if(((mode>>6)&0x03) == 2)
	Cmdhandler.DataMode=QSPI_DATA_2_LINES;             			//数据模式
	else if(((mode>>6)&0x03) == 3)
	Cmdhandler.DataMode=QSPI_DATA_4_LINES;             			//数据模式
	
	Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;				//每次都发送指令
	Cmdhandler.AlternateByteMode=QSPI_ALTERNATE_BYTES_NONE;		//无交替字节
	Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;					//关闭DDR模式
	Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

	HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
}

//QSPI接收指定长度的数据
//buf:接收数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                           //配置数据长度
    if(HAL_QSPI_Receive(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //接收数据
    else return 1;
}

//QSPI发送指定长度的数据
//buf:发送数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                            //配置数据长度
    if(HAL_QSPI_Transmit(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //发送数据
    else return 1;
}

uint32_t QSPI_EnableMemoryMappedMode(void)
{
  QSPI_CommandTypeDef cmd = {0};
  QSPI_MemoryMappedTypeDef mem = {0};

  // Configure the command for the read instruction
  cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  cmd.Instruction = W25X_FastReadQuadIO;

  cmd.AddressMode = QSPI_ADDRESS_4_LINES;
  cmd.AddressSize = QSPI_ADDRESS_24_BITS;

  cmd.DataMode = QSPI_DATA_4_LINES;

  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

  cmd.DummyCycles = 8;

  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // Configure the memory mapped mode
  mem.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  mem.TimeOutPeriod = 0;

  if (HAL_QSPI_MemoryMapped(&QSPI_Handler, &cmd, &mem) != HAL_OK)
  {
    return 1;
  }

  return 0;
}

void QSPI_DeInit(void)
{
    HAL_QSPI_Abort(&QSPI_Handler);
    HAL_QSPI_DeInit(&QSPI_Handler);
}

HAL_StatusTypeDef QSPI_Command(QSPI_CommandTypeDef *cmd, uint32_t Timeout)
{
	return HAL_QSPI_Command(&QSPI_Handler, cmd, Timeout);
}



