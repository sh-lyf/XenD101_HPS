/**
  ******************************************************************************
  * @file    stm32_qspi.c
  * @author  iclm team
  * @brief   qspi services
  ******************************************************************************
  */
#include "w25qxx.h"
#include "stm32_qspi.h"

static QSPI_HandleTypeDef QSPI_Handler;    //QSPI���

//�ȴ�״̬��־
//flag:��Ҫ�ȴ��ı�־λ
//sta:��Ҫ�ȴ���״̬
//wtime:�ȴ�ʱ��
//����ֵ:0,�ȴ��ɹ�.
//	     1,�ȴ�ʧ��.
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

//QSPI��ʼ��
uint8_t QSPI_Init(void)
{
    QSPI_Handler.Instance=QUADSPI;                          //QSPI
    QSPI_Handler.Init.ClockPrescaler=1;                     //QPSI��Ƶ�ȣ�W25Q256���Ƶ��Ϊ104M��
                                                            //���Դ˴�Ӧ��Ϊ2��QSPIƵ�ʾ�Ϊ200/(1+1)=100MHZ
    QSPI_Handler.Init.FifoThreshold=4;                      //FIFO��ֵΪ4���ֽ�
    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;//������λ�������(DDRģʽ��,��������Ϊ0)
    QSPI_Handler.Init.FlashSize=POSITION_VAL(0X800000)-1;  //SPI FLASH��С��W25Q64��СΪ8M�ֽ�
    QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;//Ƭѡ�ߵ�ƽʱ��Ϊ5��ʱ��(10*5=55ns),���ֲ������tSHSL����
    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          //ģʽ0
    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_1;              //��һƬflash
    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE;     //��ֹ˫����ģʽ
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) return 0;      //QSPI��ʼ���ɹ�
    else return 1;
}

//QSPI�ײ�����,�������ã�ʱ��ʹ��
//�˺����ᱻHAL_QSPI_Init()����
//hqspi:QSPI���
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_QSPI_CLK_ENABLE();        //ʹ��QSPIʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOBʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();       //ʹ��GPIODʱ��
	__HAL_RCC_GPIOE_CLK_ENABLE();       //ʹ��GPIODʱ��
    
    //��ʼ��PB10 Ƭѡ�ź�
    GPIO_Initure.Pin=GPIO_PIN_10;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //����
    GPIO_Initure.Pull=GPIO_PULLUP;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  //����
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;   //����ΪQSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //PD11,12
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
    //PB2
    GPIO_Initure.Pin=GPIO_PIN_2;
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;   //����ΪQSPI
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

//QSPI��������
//cmd:Ҫ���͵�ָ��
//addr:���͵���Ŀ�ĵ�ַ
//mode:ģʽ,��ϸλ��������:
//	mode[1:0]:ָ��ģʽ: 00,��ָ��;01,���ߴ���ָ��;10,˫�ߴ���ָ��;11,���ߴ���ָ��.
//	mode[3:2]:��ַģʽ: 00,�޵�ַ;01,���ߴ����ַ;10,˫�ߴ����ַ;11,���ߴ����ַ.
//	mode[5:4]:��ַ����: 00,8λ��ַ;01,16λ��ַ;10,24λ��ַ;11,32λ��ַ.
//	mode[7:6]:����ģʽ: 00,������;01,���ߴ�������;10,˫�ߴ�������;11,���ߴ�������.
//dmcycle:��ָ��������
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle)
{
	QSPI_CommandTypeDef Cmdhandler;
    
	Cmdhandler.Instruction=cmd;									//ָ��
	Cmdhandler.Address=addr;									//��ַ
	Cmdhandler.DummyCycles=dmcycle;								//���ÿ�ָ��������
	
	if(((mode>>0)&0x03) == 0)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_NONE;			//ָ��ģʽ
	else if(((mode>>0)&0x03) == 1)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_1_LINE;			//ָ��ģʽ
	else if(((mode>>0)&0x03) == 2)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_2_LINES;			//ָ��ģʽ
	else if(((mode>>0)&0x03) == 3)
	Cmdhandler.InstructionMode=QSPI_INSTRUCTION_4_LINES;			//ָ��ģʽ
	
	if(((mode>>2)&0x03) == 0)
	Cmdhandler.AddressMode=QSPI_ADDRESS_NONE;   					//��ַģʽ
	else if(((mode>>2)&0x03) == 1)
	Cmdhandler.AddressMode=QSPI_ADDRESS_1_LINE;   					//��ַģʽ
	else if(((mode>>2)&0x03) == 2)
	Cmdhandler.AddressMode=QSPI_ADDRESS_2_LINES;   					//��ַģʽ
	else if(((mode>>2)&0x03) == 3)
	Cmdhandler.AddressMode=QSPI_ADDRESS_4_LINES;   					//��ַģʽ
	
	if(((mode>>4)&0x03) == 0)
	Cmdhandler.AddressSize=QSPI_ADDRESS_8_BITS;   					//��ַ����
	else if(((mode>>4)&0x03) == 1)
	Cmdhandler.AddressSize=QSPI_ADDRESS_16_BITS;   					//��ַ����
	else if(((mode>>4)&0x03) == 2)
	Cmdhandler.AddressSize=QSPI_ADDRESS_24_BITS;   					//��ַ����
	else if(((mode>>4)&0x03) == 3)
	Cmdhandler.AddressSize=QSPI_ADDRESS_32_BITS;   					//��ַ����
	
	if(((mode>>6)&0x03) == 0)
	Cmdhandler.DataMode=QSPI_DATA_NONE;             			//����ģʽ
	else if(((mode>>6)&0x03) == 1)
	Cmdhandler.DataMode=QSPI_DATA_1_LINE;             			//����ģʽ
	else if(((mode>>6)&0x03) == 2)
	Cmdhandler.DataMode=QSPI_DATA_2_LINES;             			//����ģʽ
	else if(((mode>>6)&0x03) == 3)
	Cmdhandler.DataMode=QSPI_DATA_4_LINES;             			//����ģʽ
	
	Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;				//ÿ�ζ�����ָ��
	Cmdhandler.AlternateByteMode=QSPI_ALTERNATE_BYTES_NONE;		//�޽����ֽ�
	Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;					//�ر�DDRģʽ
	Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

	HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                           //�������ݳ���
    if(HAL_QSPI_Receive(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //��������
    else return 1;
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                            //�������ݳ���
    if(HAL_QSPI_Transmit(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //��������
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



