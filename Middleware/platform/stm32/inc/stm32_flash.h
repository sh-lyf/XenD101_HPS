/**
  ******************************************************************************
  * @file    stm32_flash.h
  * @author  ting.gao@iclegend.com
  * @brief   flash header file
  ******************************************************************************
  */
#ifndef __STM32_FLASH_H__
#define __STM32_FLASH_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#if defined(STM32F429xx)
#include "stm32f4xx_hal.h"
#endif

#if defined(STM32H750xx)
#include "stm32h7xx_hal.h"
#include "w25qxx.h"
#endif


/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */

#if defined(STM32F429xx)
#define FMC_START_ADDR                (0x08000000U)
#define FMC_END_ADDR                  (0x08080000U)

#define FMC_PAGE_SIZE                 (0x20000U)
#define FMC_SW_SETTING_ADDR           ADDR_FLASH_SECTOR_7
#define FMC_HW_SETTING_ADDR           ADDR_FLASH_SECTOR_6
#endif

#if defined(STM32H750xx)
#define FMC_START_ADDR                (0x00000000U)
#define FMC_END_ADDR                  (0x00800000U)

#define FMC_PAGE_SIZE                 (0x1000U)
#define FMC_SW_SETTING_ADDR           (FMC_END_ADDR - FMC_PAGE_SIZE)
#define FMC_HW_SETTING_ADDR           (FMC_END_ADDR - FMC_PAGE_SIZE * 2)
#endif

int8_t Flash_ErasePage(uint32_t pageAddr);
int8_t Flash_Program(uint32_t flashStartAddr, uint32_t *data, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif

