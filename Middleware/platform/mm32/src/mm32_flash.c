/**
  ******************************************************************************
  * @file    gd32_flash.c
  * @author  ting.gao@iclegend.com
  * @brief   flash services
  ******************************************************************************
  */
#include "mm32_flash.h"
#include "utilities.h"

int8_t Flash_ErasePage(uint32_t pageAddr)
{
    if (pageAddr % FMC_PAGE_SIZE)
    {
        return FAIL;
    }
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(pageAddr);
    FLASH_ClearFlag(FLASH_FLAG_EOP );
    FLASH_Lock();

    return OK;
}

int8_t Flash_Program(uint32_t flashStartAddr, uint32_t *data, uint16_t len)
{
    uint16_t loop = 0;
    
    if ((flashStartAddr + (len * sizeof(uint32_t))) > FMC_END_ADDR)
    {
        return FAIL;
    }

    if (flashStartAddr % sizeof(uint32_t))
    {
        return FAIL;
    }
    
    FLASH_Unlock();

    for (loop = 0; loop < len; loop++)
    {
        FLASH_ProgramWord(flashStartAddr, data[loop]);
        flashStartAddr += sizeof(uint32_t);
        FLASH_ClearFlag(FLASH_FLAG_EOP );
    }
    
    FLASH_Lock();

    return OK;
}


