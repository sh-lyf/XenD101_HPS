/**
  ******************************************************************************
  * @file    gd32_flash.h
  * @author  ting.gao@iclegend.com
  * @brief   flash header file
  ******************************************************************************
  */
#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "hal_conf.h"

#define FMC_START_ADDR                (0x08000000U)
#define FMC_END_ADDR                  (0x08004000U)

#define FMC_PAGE_SIZE                 (0x400U)
#define FMC_SW_SETTING_ADDR           (0x08003C00U)
#define FMC_HW_SETTING_ADDR           (0x08003800U)

int8_t Flash_ErasePage(uint32_t pageAddr);
int8_t Flash_Program(uint32_t flashStartAddr, uint32_t *data, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif

