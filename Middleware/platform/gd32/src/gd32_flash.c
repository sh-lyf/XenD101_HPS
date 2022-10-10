/**
  ******************************************************************************
  * @file    gd32_flash.c
  * @author  ting.gao@iclegend.com
  * @brief   flash services
  ******************************************************************************
  */
#include "gd32_flash.h"
#include "utilities.h"

int8_t Flash_ErasePage(uint32_t pageAddr)
{
    if (pageAddr % FMC_PAGE_SIZE)
    {
        return FAIL;
    }
    
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    fmc_page_erase(pageAddr);
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    fmc_lock();
    
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
    
    fmc_unlock();

    for (loop = 0; loop < len; loop++)
    {
        fmc_word_program(flashStartAddr, data[loop]);
        flashStartAddr += sizeof(uint32_t);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }
    
    fmc_lock();

    return OK;
}


