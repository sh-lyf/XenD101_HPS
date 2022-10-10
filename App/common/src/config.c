/**
  ******************************************************************************
  * @file           : config.c
  * @author         : ting.gao@iclegend.com
  * @brief          : config module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "utilities.h"
#include "radar.h"
#include "dataprocess.h"
#include "system.h"
#if defined(SUPPORT_MOTION_TARGET_TRACKING) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "mtt_task.h"
#include "motion_target_tracking.h"
#endif
#if defined(SUPPORT_VITAL_SIGN) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "vs_config.h"
#endif
#if defined(SUPPORT_GESTURE_RECOGNITION) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "gr_task.h"
#endif
#ifdef SUPPORT_ABD
#include "abd.h"
#endif

static FLASH_DATA_T flashData;
static uint8_t needFlashWrite = 0;

void Config_WriteData2Flash(void)
{
    uint32_t flashAddr = FLASH_DATA_PAGE_ADDR;
    uint16_t idx = 0;

#ifdef DEVMB
    QSPI_DeInit();
    W25QXX_Init();
#endif
    Flash_ErasePage(FLASH_DATA_PAGE_ADDR);

    flashData.magic = FLASH_MAGIC_NUM;
    Flash_Program(flashAddr, &flashData.magic, 1);
    flashAddr += FLASH_WORD_LEN;

    for (idx = 0; idx < FLASH_ELEM_MAX; idx++)
    {
        Flash_Program(flashAddr, &flashData.elem[idx].elemLen, 1);
        flashAddr += FLASH_WORD_LEN;
        Flash_Program(flashAddr, flashData.elem[idx].elemAddr, flashData.elem[idx].elemLen/FLASH_WORD_LEN);
        flashAddr += flashData.elem[idx].elemLen;
    }
#ifdef DEVMB
    QSPI_DeInit();
    W25QXX_Init();
    QSPI_EnableMemoryMappedMode();
#endif
}

void Config_RetrieveFlashData(void)
{
    uint16_t idx = 0;
    uint16_t loop = 0;
    uint16_t readNum = 0;
    uint32_t *readAddr = NULL;
    uint32_t flashAddr = FLASH_DATA_PAGE_ADDR;

#ifdef DEVMB
    flashAddr += QSPI_MEM_START;
#endif
    flashData.magic = REG32(flashAddr);
    if (FLASH_MAGIC_NUM != flashData.magic)
    {
        printf("flash setting is empty!\r\n");
        Config_WriteData2Flash();
#ifdef STM32_PLATFORM
        CACHE_InvalDCache((uint32_t*)flashAddr, sizeof(uint32_t));
#endif
    }

    flashData.magic = REG32(flashAddr);
    if (FLASH_MAGIC_NUM != flashData.magic)
    {
        printf("Error: flash work abnormal!\r\n");
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
    
    flashAddr += FLASH_WORD_LEN;
    for (idx = 0; idx < FLASH_ELEM_MAX; idx++)
    {
        flashData.elem[idx].elemLen = REG32(flashAddr);
        flashAddr += FLASH_WORD_LEN;
        
        readNum = flashData.elem[idx].elemLen/FLASH_WORD_LEN;
        readAddr = (uint32_t *)flashData.elem[idx].elemAddr;
        for (loop = 0; loop < readNum; loop++)
        {
            readAddr[loop] = REG32(flashAddr);
            flashAddr += FLASH_WORD_LEN;
        }
    }
}

void Config_SavePara2Flash(void)
{    
    if (!needFlashWrite)
    {
        return;
    }

    Config_WriteData2Flash();
    needFlashWrite = 0;
}

void Config_EarseFlashData(void)
{    
    Flash_ErasePage(FLASH_DATA_PAGE_ADDR);
}

void Config_NeedFlashWrite(void)
{
    needFlashWrite = 1;
}

uint32_t Config_GetSN(void)
{
    uint32_t flashAddr = FLASH_SN_ADDR;

#ifdef DEVMB
    flashAddr += QSPI_MEM_START;
#endif
    return REG32(flashAddr);
}

void Config_Init(void)
{
    uint16_t totalLen = 0;

    totalLen += sizeof(flashData.magic);
    
    flashData.elem[FLASH_ELEM_RADAR].elemAddr = Radar_GetRadarParaAddr(); 
    flashData.elem[FLASH_ELEM_RADAR].elemLen = Radar_GetRadarParaLen();
    totalLen += flashData.elem[FLASH_ELEM_RADAR].elemLen + sizeof(flashData.elem[FLASH_ELEM_RADAR].elemLen);

    System_ParaInit();
    flashData.elem[FLASH_ELEM_SYS].elemAddr = System_GetSysParaAddr(); 
    flashData.elem[FLASH_ELEM_SYS].elemLen = System_GetSysParaLen();
    totalLen += flashData.elem[FLASH_ELEM_SYS].elemLen + sizeof(flashData.elem[FLASH_ELEM_SYS].elemLen);
#if defined(SUPPORT_DFFT_PEAK_CONFIG)
    flashData.elem[FLASH_ELEM_PEAK].elemAddr = PEAK_GetSysParaAddr(); 
    flashData.elem[FLASH_ELEM_PEAK].elemLen = PEAK_GetSysParaLen();
    totalLen += flashData.elem[FLASH_ELEM_PEAK].elemLen + sizeof(flashData.elem[FLASH_ELEM_PEAK].elemLen);
#endif

#if defined(SUPPORT_ABD)
    flashData.elem[FLASH_ELEM_ABD].elemAddr = ABD_GetParaAddr(); 
    flashData.elem[FLASH_ELEM_ABD].elemLen = ABD_GetParaLen();
    totalLen += flashData.elem[FLASH_ELEM_ABD].elemLen + sizeof(flashData.elem[FLASH_ELEM_ABD].elemLen);
#endif

#if defined(SUPPORT_MOTION_TARGET_TRACKING) || defined(SUPPORT_DYNAMIC_SYS_MODE)
    MTT_ParaInit();
    flashData.elem[FLASH_ELEM_MTT].elemAddr = MTT_GetMttParaAddr(); 
    flashData.elem[FLASH_ELEM_MTT].elemLen = MTT_GetMttParaLen();
    totalLen += flashData.elem[FLASH_ELEM_MTT].elemLen + sizeof(flashData.elem[FLASH_ELEM_MTT].elemLen);
#endif  

#if defined(SUPPORT_VITAL_SIGN) || defined(SUPPORT_DYNAMIC_SYS_MODE)
    VS_ParaInit();
#endif

#if defined(SUPPORT_GESTURE_RECOGNITION) || defined(SUPPORT_DYNAMIC_SYS_MODE)
	GR_ParaInit();
#endif

    if (totalLen > FLASH_PAGE_SIZE)
    {
        printf("Error: flashDataLen is more than FLASH_PAGE_SIZE!\r\n");
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    Config_RetrieveFlashData();
    printf("sn: 0x%x\r\n", Config_GetSN());
}



