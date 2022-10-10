/**
  ******************************************************************************
  * @file    gd32_i2c.c
  * @author  ting.gao@iclegend.com
  * @brief   i2c services
  ******************************************************************************
  */
#include "gd32_i2c.h"
#include "utilities.h"

static void GD32F3X0_I2cWrite(uint32_t i2c_periph, uint32_t devAddr, uint8_t regAddr, uint8_t* pBuffer,  uint8_t size)
{
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));
    
    i2c_start_on_bus(i2c_periph);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    
    i2c_master_addressing(i2c_periph, devAddr, I2C_TRANSMITTER);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    i2c_flag_clear(i2c_periph,I2C_FLAG_ADDSEND);
    
    while( SET != i2c_flag_get(i2c_periph, I2C_FLAG_TBE));
    
    i2c_data_transmit(i2c_periph, regAddr);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
    
    while(size--)
    {  
        i2c_data_transmit(i2c_periph, *pBuffer); 
        pBuffer++; 
        while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
    }

    i2c_stop_on_bus(i2c_periph);
    while(I2C_CTL0(i2c_periph)&0x0200);
}

static void GD32F3X0_I2cRead(uint32_t i2c_periph, uint32_t devAddr, uint8_t regAddr, uint8_t* pBuffer,  uint8_t size)
{  
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    if(2 == size)
    {
        i2c_ackpos_config(i2c_periph,I2C_ACKPOS_NEXT);
    }
    
    i2c_start_on_bus(i2c_periph);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    
    i2c_master_addressing(i2c_periph, devAddr, I2C_TRANSMITTER);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    i2c_flag_clear(i2c_periph,I2C_FLAG_ADDSEND);
    
    while(SET != i2c_flag_get( i2c_periph , I2C_FLAG_TBE));
 
    i2c_data_transmit(i2c_periph, regAddr);  
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
    
    i2c_start_on_bus(i2c_periph);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    
    i2c_master_addressing(i2c_periph, devAddr, I2C_RECEIVER);

    if(size < 3)
    {
        i2c_ack_config(i2c_periph,I2C_ACK_DISABLE);
    }
    
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    i2c_flag_clear(i2c_periph,I2C_FLAG_ADDSEND);
    
    if(1 == size)
    {
        i2c_stop_on_bus(i2c_periph);
    }
    
    while(size)
    {
        if(3 == size)
        {
            while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
            i2c_ack_config(i2c_periph,I2C_ACK_DISABLE);
        }
        
        if(2 == size)
        {
            while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
            i2c_stop_on_bus(i2c_periph);
        }
        
        if(i2c_flag_get(i2c_periph, I2C_FLAG_RBNE))
        {
            *pBuffer = i2c_data_receive(i2c_periph);
            pBuffer++; 
            size--;
        } 
    }
    
    while(I2C_CTL0(i2c_periph)&0x0200);
    
    i2c_ack_config(i2c_periph,I2C_ACK_ENABLE);
    i2c_ackpos_config(i2c_periph,I2C_ACKPOS_CURRENT);
}

uint16_t I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal)
{
    regVal = LE16_TO_BIG(regVal);
    
    GD32F3X0_I2cWrite(I2C1, devAddr, regAddr, (uint8_t*)&regVal,  sizeof(regVal));
	
	return 0;
}


uint16_t I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal)
{  
    uint16_t val = 0;
    
    GD32F3X0_I2cRead(I2C1, devAddr, regAddr, (uint8_t*)&val,  sizeof(val));

    *regVal = BIG16_TO_LE(val);
	return 0;
}

uint32_t I2C_GetConfigBySpeed(uint8_t I2CSpeed)
{
	uint32_t Config = gd32_I2C_Speed_Config_400k;
	switch(I2CSpeed)
	{
		case I2C_Speed_100k:
		{
			Config = gd32_I2C_Speed_Config_100k;
			break;
		}
		case I2C_Speed_200k:
		{
			Config = gd32_I2C_Speed_Config_200k;
			break;
		}
		case I2C_Speed_300k:
		{
			Config = gd32_I2C_Speed_Config_300k;
			break;
		}
		case I2C_Speed_400k:
		{
			Config = gd32_I2C_Speed_Config_400k;
			break;
		}
		case I2C_Speed_1M:
		{
			Config = gd32_I2C_Speed_Config_1M;
			break;
		}
		default:
		{
			break;
		}
	}
	return Config;
}

void I2C_Init(uint8_t I2CSpeed)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_0 | GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_0 | GPIO_PIN_1);

    rcu_periph_clock_enable(RCU_I2C1);
    
	uint32_t ConfigSpeed = I2C_GetConfigBySpeed(I2CSpeed);
    i2c_clock_config(I2C1,ConfigSpeed,I2C_DTCY_2);
    i2c_mode_addr_config(I2C1,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,I2C_SLAVE_ADDRESS7);
    i2c_enable(I2C1);
    i2c_ack_config(I2C1,I2C_ACK_ENABLE);
}


