/**
  ******************************************************************************
  * @file           : system.c
  * @author         : iclm team
  * @brief          : system module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "global_conf.h"
#include "utilities.h"
#include "system.h"
#include "platform.h"
#include "dataprocess.h"
#include "usb_transfer.h"

static SYS_PARA_T sysPara __ALIGN(4);
static uint8_t needReconfig = 0;

void* System_GetSysParaAddr(void)
{
    return (void*)&sysPara;
}

uint32_t System_GetSysParaLen(void)
{
    return sizeof(sysPara);
}

uint16_t System_GetSysMode(void)
{
    return sysPara.systemMode;
}

uint16_t System_GetUploadSampleRate(void)
{
    return sysPara.uploadSampleRate;
}

void System_ParaInit(void)
{
//#ifdef SUPPORT_DYNAMIC_SYS_MODE
#ifdef FFT_HARDWARE_TEST
    sysPara.systemMode = SYS_MODE_PASSTHROUGH;
#else
    sysPara.systemMode = SYS_MODE_DEFAULT;
#endif
//#endif
    sysPara.uploadSampleRate = UPLOAD_SAMPLE_RATE;
}

int8_t System_ParaUpdate(uint16_t type, int32_t val)
{
    switch (type)  
    {
//#ifdef SUPPORT_DYNAMIC_SYS_MODE
        case SYS_SYSTEM_MODE:
            if (sysPara.systemMode != (int16_t)val)
            {
                needReconfig = 1;
            }
            sysPara.systemMode = (int16_t)val;
            break;
//#endif
			
#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
        case SYS_UPLOAD_SP_RATE:
            sysPara.uploadSampleRate = (int16_t)val;
            DataProc_ResetRecvCnt();
            break;
#endif
        
        default:
            return -1;
    }

    return 0;
}

int32_t System_ParaRead(uint16_t type)
{
    switch (type)  
    {
        case SYS_SYSTEM_MODE:
            return sysPara.systemMode;

        case SYS_UPLOAD_SP_RATE:
            return sysPara.uploadSampleRate;

        default:
            return 0x7fffffff; /*invalid value*/
    }
}

void System_Reconfig(void)
{
    uint16_t dmaBufLen = 0;

#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    printf("system reconfig check...\r\n");
#endif
    if (!needReconfig)
    {
        needReconfig = DataProc_NeedReconfig();
    }

    if (!needReconfig)
    {
        return;
    }
    needReconfig = 0;

#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    printf("system now do reconfig!\r\n");
#endif    
    SPI_DeInit();
#ifdef STM32_PLATFORM
	UsbTransfer_ResetUsbBuffer();
    if (DATA_TYPE_DFFT_PEAK == DataProc_GetRadarDataType())
    {
        UsbTransfer_DecUsbTxLen();
    }
#endif
    dmaBufLen = DataProc_GetRadarDataLen() * 2;
    SPI_Init(dmaBufLen);
}


