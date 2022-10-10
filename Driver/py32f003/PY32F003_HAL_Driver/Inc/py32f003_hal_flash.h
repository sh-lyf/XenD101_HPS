/**
  ******************************************************************************
  * @file    PY32F003_hal_flash.h
  * @author  MCU Application Team
  * @brief   Header file of FLASH HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software is owned and published by:
  * Puya Semiconductor Co., Ltd.
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  *  1. Redistributions of source code must retain the above copyright notice,
  *     this list of conditions and the following disclaimer.
  *  2. Redistributions in binary form must reproduce the above copyright notice,
  *     this list of conditions and the following disclaimer in the documentation
  *     and/or other materials provided with the distribution.
  *  3. Neither the name of the copyright holder nor the names of its contributors
  *     may be used to endorse or promote products derived from this software
  *     without specific prior written permission.
  *
  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  *  POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PY32F003_HAL_FLASH_H
#define __PY32F003_HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f003_hal_def.h"

/** @addtogroup PY32F003_HAL_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup FLASH_Exported_Types FLASH Exported Types
  * @{
  */

/**
  * @brief  FLASH Erase structure definition
  */
typedef struct
{
    uint32_t TypeErase;   /*!< Mass erase or page erase.
                             This parameter can be a value of @ref FLASH_Type_Erase */
    uint32_t PageAddress;      /*!< PageAdress: Initial FLASH page address to erase when mass erase is disabled
                             This parameter must be a number between Min_Data = FLASH_BASE and Max_Data = FLASH_BANK1_END */
    uint32_t SectorAddress;
    uint32_t NbPages;     /*!< Number of pages to be erased.
                             This parameter must be a value between 1 and (FLASH_PAGE_NB - value of initial page)*/
} FLASH_EraseInitTypeDef;

/**
  * @brief  FLASH Option Bytes PROGRAM structure definition
  */
typedef struct
{
  uint32_t  OptionType;       /*!< OptionType: Option byte to be configured.
                                   This parameter can be a value of @ref FLASHEx_Option_Type */

  uint32_t  WRPState;         /*!< WRPState: Write protection activation or deactivation.
                                   This parameter can be a value of @ref FLASHEx_WRP_State */

  uint32_t  WRPSector;        /*!< WRPSector: This bitfield specifies the sector (s) which are write protected.
                                   This parameter can be a combination of @ref FLASHEx_Option_Bytes_Write_Protection */

  uint32_t  SDKState;         /*!< SDKState: Write protection activation or deactivation.
                                   This parameter can be a value of @ref FLASHEx_SDK_State */

  uint32_t  SDKStartAddr;     /*!< SDK Start address (used for FLASH_SDKR). It represents first address of start block
                                   to protect. Make sure this parameter is multiple of SDK granularity: 2048 Bytes.*/
    
  uint32_t  SDKEndAddr;       /*!< SDK End address (used for FLASH_SDKR). It represents first address of end block
                                   to protect. Make sure this parameter is multiple of SDK granularity: 2048 Bytes.*/
    
  uint32_t   RDPLevel;        /*!< RDPLevel: Set the read protection level.
                                   This parameter can be a value of @ref FLASHEx_Option_Bytes_Read_Protection */

  uint32_t   BORLevel;        /*!< BORLevel: Set the BOR Level.
                                   This parameter can be a value of @ref FLASHEx_Option_Bytes_BOR_Level */

  uint32_t   USERConfig;      /*!< USERConfig: Program the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.
                                   This parameter can be a combination of 
                                   @ref FLASHEx_Option_Bytes_IWatchdog,
                                   @ref FLASHEx_Option_Bytes_WWatchdog,  
                                   @ref FLASHEx_Option_Bytes_USER_RESET_CONFIG*/

  uint32_t BOOTBit1Config;    /*!< BOOT1Config: Together with input pad Boot0, this bit selects the boot source, flash, ram or system memory
                                   This parameter can be a value of @ref FLASHEx_Option_Bytes_BOOTBit1 */
} FLASH_OBProgramInitTypeDef;

/**
* @brief  FLASH handle Structure definition
*/
typedef struct
{
    HAL_LockTypeDef   Lock;              /* FLASH locking object */
    uint32_t          ErrorCode;         /* FLASH error code */
    uint32_t          ProcedureOnGoing;  /* Internal variable to indicate which procedure is ongoing or not in IT context */
    uint32_t          Address;           /* Internal variable to save address selected for program in IT context */
    uint32_t          Page;              /* Internal variable to define the current page which is erasing in IT context */
    uint32_t          NbPagesToErase;    /* Internal variable to save the remaining pages to erase in IT context */
} FLASH_ProcessTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */

/**
  * @}
  */

/** @defgroup FLASH_Latency FLASH Latency
  * @{
  */
#define FLASH_LATENCY_0                 0x00000000UL                                /*!< FLASH Zero wait state */
#define FLASH_LATENCY_1                 FLASH_ACR_LATENCY                        /*!< FLASH One wait state */

#define FLASH_TYPEERASE_PAGES           (0x00U)  /*!<Pages erase only*/
#define FLASH_TYPEERASE_MASSERASE       (0x01U)  /*!<Flash mass erase activation*/
#define FLASH_TYPEERASE_PAGEERASE       (0x02U)  /*!<Flash mass erase activation*/
#define FLASH_TYPEERASE_SECTORERASE     (0x03U)
/**
  * @}
  */

/** @defgroup FLASH_Flags FLASH Flags Definition
  * @{
  */
//#define FLASH_FLAG_OPERR                FLASH_SR_OPERR    /*!< FLASH Operation error flag */
//#define FLASH_FLAG_PROGERR              FLASH_SR_PROGERR  /*!< FLASH Programming error flag */
//#define FLASH_FLAG_PGAERR               FLASH_SR_PGAERR   /*!< FLASH Programming alignment error flag */
//#define FLASH_FLAG_SIZERR               FLASH_SR_SIZERR   /*!< FLASH Size error flag  */
//#define FLASH_FLAG_PGSERR               FLASH_SR_PGSERR   /*!< FLASH Programming sequence error flag */
//#define FLASH_FLAG_MISERR               FLASH_SR_MISERR   /*!< FLASH Fast programming data miss error flag */
//#define FLASH_FLAG_FASTERR              FLASH_SR_FASTERR  /*!< FLASH Fast programming error flag */
#if defined(FLASH_PCROP_SUPPORT)
//#define FLASH_FLAG_RDERR                FLASH_SR_RDERR    /*!< FLASH PCROP read error flag */
#endif /* FLASH_PCROP_SUPPORT */

#define FLASH_FLAG_BSY                  FLASH_SR_BSY      /*!< FLASH Operation Busy flag */
#define FLASH_FLAG_OPTVERR              FLASH_SR_OPTVERR  /*!< FLASH Option validity error flag */
#define FLASH_FLAG_WRPERR               FLASH_SR_WRPERR   /*!< FLASH Write protection error flag */
#define FLASH_FLAG_EOP                  FLASH_SR_EOP      /*!< FLASH End of operation flag */

#if defined(FLASH_PCROP_SUPPORT)
#define FLASH_FLAG_ALL_ERRORS           (FLASH_FLAG_OPERR   | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | \
                                         FLASH_FLAG_PGAERR  | FLASH_FLAG_SIZERR  | FLASH_FLAG_PGSERR | \
                                         FLASH_FLAG_MISERR  | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR  | \
                                         FLASH_FLAG_OPTVERR | FLASH_FLAG_ECCC    | FLASH_FLAG_ECCD)
#else
#define FLASH_FLAG_ALL_ERRORS           (FLASH_FLAG_OPERR   | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | \
                                         FLASH_FLAG_PGAERR  | FLASH_FLAG_SIZERR  | FLASH_FLAG_PGSERR | \
                                         FLASH_FLAG_MISERR  | FLASH_FLAG_FASTERR | \
                                         FLASH_FLAG_OPTVERR | FLASH_FLAG_ECCC    | FLASH_FLAG_ECCD)

#endif /* FLASH_PCROP_SUPPORT */
/**
  * @}
  */

/** @defgroup FLASH_Interrupt_definition FLASH Interrupts Definition
  * @brief FLASH Interrupt definition
  * @{
  */
#define FLASH_IT_EOP                    FLASH_CR_EOPIE              /*!< End of FLASH Operation Interrupt source */
#define FLASH_IT_OPERR                  FLASH_CR_ERRIE              /*!< Error Interrupt source */
#if defined(FLASH_PCROP_SUPPORT)
#define FLASH_IT_RDERR                  FLASH_CR_RDERRIE            /*!< PCROP Read Error Interrupt source*/
#endif
#define FLASH_IT_ECCC                   (FLASH_ECCR_ECCCIE >> FLASH_ECCR_ECCCIE_Pos)   /*!< ECC Correction Interrupt source */
/**
  * @}
  */

/** @defgroup FLASH_Error FLASH Error
  * @{
  */
#define HAL_FLASH_ERROR_NONE            0x00000000U
#define HAL_FLASH_ERROR_OP              FLASH_FLAG_OPERR
#define HAL_FLASH_ERROR_PROG            FLASH_FLAG_PROGERR
#define HAL_FLASH_ERROR_WRP             FLASH_FLAG_WRPERR
#define HAL_FLASH_ERROR_PGA             FLASH_FLAG_PGAERR
#define HAL_FLASH_ERROR_SIZ             FLASH_FLAG_SIZERR
#define HAL_FLASH_ERROR_PGS             FLASH_FLAG_PGSERR
#define HAL_FLASH_ERROR_MIS             FLASH_FLAG_MISERR
#define HAL_FLASH_ERROR_FAST            FLASH_FLAG_FASTERR
#if defined(FLASH_PCROP_SUPPORT)
#define HAL_FLASH_ERROR_RD              FLASH_FLAG_RDERR
#endif
#define HAL_FLASH_ERROR_OPTV            FLASH_FLAG_OPTVERR
#define HAL_FLASH_ERROR_ECCD            FLASH_FLAG_ECCD
/**
  * @}
  */

/** @defgroup FLASH_PROGRAM_ERASE_CLOCK FLASH Program and Erase Clock
  * @{
  */
#define FLASH_PROGRAM_ERASE_CLOCK_4MHZ        0x00000000U           /*!< 4MHz */
#define FLASH_PROGRAM_ERASE_CLOCK_8MHZ        0x00000001U           /*!< 8MHz */
#define FLASH_PROGRAM_ERASE_CLOCK_16MHZ       0x00000002U           /*!< 16MHz */
#define FLASH_PROGRAM_ERASE_CLOCK_22P12MHZ    0x00000003U           /*!< 22.12MHz */
#define FLASH_PROGRAM_ERASE_CLOCK_24MHZ       0x00000004U           /*!< 24MHz */
/**
  * @}
  */


/** @defgroup FLASHEx_Option_Bytes_Write_Protection FLASH Option Bytes Write Protection
  * @{
  */
#define OB_WRP_Pages0to31          ((uint32_t)0x00000001U) /* Write protection of Sector0 */
#define OB_WRP_Pages32to63         ((uint32_t)0x00000002U) /* Write protection of Sector1 */
#define OB_WRP_Pages64to95         ((uint32_t)0x00000004U) /* Write protection of Sector2 */
#define OB_WRP_Pages96to127        ((uint32_t)0x00000008U) /* Write protection of Sector3 */
#define OB_WRP_Pages128to159       ((uint32_t)0x00000010U) /* Write protection of Sector4 */
#define OB_WRP_Pages160to191       ((uint32_t)0x00000020U) /* Write protection of Sector5 */
#define OB_WRP_Pages192to223       ((uint32_t)0x00000040U) /* Write protection of Sector6 */
#define OB_WRP_Pages224to255       ((uint32_t)0x00000080U) /* Write protection of Sector7 */
#define OB_WRP_Pages256to287       ((uint32_t)0x00000100U) /* Write protection of Sector8 */
#define OB_WRP_Pages288to319       ((uint32_t)0x00000200U) /* Write protection of Sector9 */
#define OB_WRP_Pages320to351       ((uint32_t)0x00000400U) /* Write protection of Sector10 */
#define OB_WRP_Pages352to383       ((uint32_t)0x00000800U) /* Write protection of Sector11 */
#define OB_WRP_Pages384to415       ((uint32_t)0x00001000U) /* Write protection of Sector12 */
#define OB_WRP_Pages416to447       ((uint32_t)0x00002000U) /* Write protection of Sector13 */
#define OB_WRP_Pages448to479       ((uint32_t)0x00004000U) /* Write protection of Sector14 */
#define OB_WRP_Pages480to511       ((uint32_t)0x00008000U) /* Write protection of Sector15 */
#define OB_WRP_AllPages            ((uint32_t)0x0000FFFFU) /*!< Write protection of all Sectors */
/**
  * @}
  */
  
/** @defgroup FLASHEx_Option_Bytes_Read_Protection FLASHEx Option Bytes Read Protection
  * @{
  */
#define OB_RDP_LEVEL_0         ((uint8_t)0xAAU)
#define OB_RDP_LEVEL_1         ((uint8_t)0x55U)

/**
  * @}
  */
#define FLASH_TYPEPROGRAM_PAGE       (0x01U)  /*!<Program 128bytes at a specified address.*/

/** @defgroup FLASHEx_Option_Bytes_BOR_Level FLASHEx Option Bytes BOR Level
  * @{
  */

#define OB_BOR_OFF       ((uint32_t)0x0000U) /*!< BOR is disabled at power down, the reset is asserted when the VDD
                                              power supply reaches the PDR(Power Down Reset) threshold (1.65V)  */
#define OB_BOR_LEVEL1    ((uint32_t)0x0100U) /*!< BOR Reset threshold levels for 1.7V - 1.8V VDD power supply    */
#define OB_BOR_LEVEL2    ((uint32_t)0x0300U) /*!< BOR Reset threshold levels for 1.9V - 2.0V VDD power supply    */
#define OB_BOR_LEVEL3    ((uint32_t)0x0500U) /*!< BOR Reset threshold levels for 2.1V - 2.2V VDD power supply    */
#define OB_BOR_LEVEL4    ((uint32_t)0x0700U) /*!< BOR Reset threshold levels for 2.3V - 2.4V VDD power supply    */
#define OB_BOR_LEVEL5    ((uint32_t)0x0900U) /*!< BOR Reset threshold levels for 2.5V - 2.6V VDD power supply    */
#define OB_BOR_LEVEL6    ((uint32_t)0x0B00U) /*!< BOR Reset threshold levels for 2.7V - 2.8V VDD power supply    */
#define OB_BOR_LEVEL7    ((uint32_t)0x0D00U) /*!< BOR Reset threshold levels for 2.9V - 3.0V VDD power supply    */
#define OB_BOR_LEVEL8    ((uint32_t)0x0F00U) /*!< BOR Reset threshold levels for 3.1V - 3.2V VDD power supply    */

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Type FLASHEx Option Type
  * @{
  */
#define OPTIONBYTE_WRP            ((uint32_t)0x01U)  /*!<WRP option byte configuration*/
#define OPTIONBYTE_SDK            ((uint32_t)0x02U)  /*!<SDK option byte configuration*/
#define OPTIONBYTE_RDP            ((uint32_t)0x04U)  /*!<RDP option byte configuration*/
#define OPTIONBYTE_USER           ((uint32_t)0x08U)  /*!<USER option byte configuration*/
#define OPTIONBYTE_BOR            ((uint32_t)0x10U)  /*!<BOR option byte configuration*/
#define OPTIONBYTE_BOOT_BIT1      ((uint32_t)0x20U)  /*!< BOOT PIN1 option byte configuration*/
#define OPTIONBYTE_ALL            (OPTIONBYTE_WRP  | \
                                   OPTIONBYTE_SDK  | \
                                   OPTIONBYTE_RDP  | \
                                   OPTIONBYTE_USER | \
                                   OPTIONBYTE_BOR  | \
                                   OPTIONBYTE_BOOT_BIT1)

/**
  * @}
  */

/** @defgroup FLASHEx_WRP_State FLASHEx WRP State
  * @{
  */
#define OB_WRPSTATE_DISABLE        ((uint32_t)0x00U)  /*!<Disable the write protection of the desired sectors*/
#define OB_WRPSTATE_ENABLE         ((uint32_t)0x01U)  /*!<Enable the write protection of the desired sectors*/

/**
  * @}
  */

/** @defgroup FLASHEx_WRP_State FLASHEx WRP State
  * @{
  */
#define OB_SDKSTATE_DISABLE        ((uint32_t)0x00U)  /*!<Disable the write protection of the desired sectors*/
#define OB_SDKSTATE_ENABLE         ((uint32_t)0x01U)  /*!<Enable the write protection of the desired sectors*/

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWatchdog FLASHEx Option Bytes IWatchdog
  * @{
  */

#define OB_IWDG_SW                     ((uint32_t)0x1000U)  /*!< Software IWDG selected */
#define OB_IWDG_HW                     ((uint32_t)0x0000U)  /*!< Hardware IWDG selected */

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_WWatchdog FLASHEx Option Bytes WWatchdog
  * @{
  */

#define OB_WWDG_SW                     ((uint32_t)0x2000U)  /*!< Software WWDG selected */
#define OB_WWDG_HW                     ((uint32_t)0x0000U)  /*!< Hardware WWDG selected */

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_USER_RESET_CONFIG FLASH Option Bytes User reset config bit
  * @{
  */
#define OB_RESET_MODE_RESET            ((uint32_t)0x0000U)  /*!< Reset pin is in Reset input mode only */
#define OB_RESET_MODE_GPIO             ((uint32_t)0x4000U)  /*!< Reset pin is in GPIO mode mode only */
/**
  * @}
  */
  
/** @defgroup FLASHEx_Option_Bytes_BOOTBit1 FLASH Option Bytes BOOT Bit1 Setup
  * @{
  */
#define OB_BOOT_BIT1_RESET      (uint32_t)(0x0000U) /*!< BOOT Bit 1 Reset */
#define OB_BOOT_BIT1_SET        (uint32_t)(0x8000U) /*!< BOOT Bit 1 Set */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup FLASH_Exported_Macros FLASH Exported Macros
  *  @brief macros to control FLASH features
  *  @{
  */

/**
  * @brief  Set the FLASH Latency.
  * @param  __LATENCY__ FLASH Latency
  *         This parameter can be one of the following values :
  *
  *     @arg @ref FLASH_ACR_LATENCY  FLASH One wait state
  *
  * @retval None
  */
#define __HAL_FLASH_SET_LATENCY(__LATENCY__)    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, (__LATENCY__))

/**
  * @brief  Get the FLASH Latency.
  * @retval FLASH Latency
  *         Returned value can be one of the following values :
  *     @arg @ref FLASH_LATENCY_0 FLASH Zero wait state
  *     @arg @ref FLASH_LATENCY_1 FLASH One wait state
  *
  */
#define __HAL_FLASH_GET_LATENCY()               READ_BIT(FLASH->ACR, FLASH_ACR_LATENCY)




/** @defgroup FLASH_Interrupt FLASH Interrupts Macros
 *  @brief macros to handle FLASH interrupts
 * @{
 */

/**
  * @brief  Enable the specified FLASH interrupt.
  * @param  __INTERRUPT__ FLASH interrupt
  *         This parameter can be any combination of the following values:
  *     @arg @ref FLASH_IT_EOP End of FLASH Operation Interrupt
  *     @arg @ref FLASH_IT_OPERR Error Interrupt
  *     @arg @ref FLASH_IT_RDERR PCROP Read Error Interrupt(*)
  *     @arg @ref FLASH_IT_ECCC ECC Correction Interrupt
  * @note (*) availability depends on devices
  * @retval none
  */
#define __HAL_FLASH_ENABLE_IT(__INTERRUPT__)    do { if(((__INTERRUPT__) & FLASH_IT_ECCC) != 0U) { SET_BIT(FLASH->ECCR, FLASH_ECCR_ECCCIE); }\
                                                     if(((__INTERRUPT__) & (~FLASH_IT_ECCC)) != 0U) { SET_BIT(FLASH->CR, ((__INTERRUPT__) & (~FLASH_IT_ECCC))); }\
                                                   } while(0U)

/**
  * @brief  Disable the specified FLASH interrupt.
  * @param  __INTERRUPT__ FLASH interrupt
  *         This parameter can be any combination of the following values:
  *     @arg @ref FLASH_IT_EOP End of FLASH Operation Interrupt
  *     @arg @ref FLASH_IT_OPERR Error Interrupt
  *     @arg @ref FLASH_IT_RDERR PCROP Read Error Interrupt(*)
  *     @arg @ref FLASH_IT_ECCC ECC Correction Interrupt
  * @note (*) availability depends on devices
  * @retval none
  */
#define __HAL_FLASH_DISABLE_IT(__INTERRUPT__)   do { if(((__INTERRUPT__) & FLASH_IT_ECCC) != 0U) { CLEAR_BIT(FLASH->ECCR, FLASH_ECCR_ECCCIE); }\
                                                     if(((__INTERRUPT__) & (~FLASH_IT_ECCC)) != 0U) { CLEAR_BIT(FLASH->CR, ((__INTERRUPT__) & (~FLASH_IT_ECCC))); }\
                                                   } while(0U)

/**
  * @brief  Check whether the specified FLASH flag is set or not.
  * @param  __FLAG__ specifies the FLASH flag to check.
  *   This parameter can be one of the following values:
  *     @arg @ref FLASH_FLAG_EOP FLASH End of Operation flag
  *     @arg @ref FLASH_FLAG_OPERR FLASH Operation error flag
  *     @arg @ref FLASH_FLAG_PROGERR FLASH Programming error flag
  *     @arg @ref FLASH_FLAG_WRPERR FLASH Write protection error flag
  *     @arg @ref FLASH_FLAG_PGAERR FLASH Programming alignment error flag
  *     @arg @ref FLASH_FLAG_SIZERR FLASH Size error flag
  *     @arg @ref FLASH_FLAG_PGSERR FLASH Programming sequence error flag
  *     @arg @ref FLASH_FLAG_MISERR FLASH Fast programming data miss error flag
  *     @arg @ref FLASH_FLAG_FASTERR FLASH Fast programming error flag
  *     @arg @ref FLASH_FLAG_RDERR FLASH PCROP read  error flag(*)
  *     @arg @ref FLASH_FLAG_OPTVERR FLASH Option validity error flag
  *     @arg @ref FLASH_FLAG_BSY FLASH write/erase operations in progress flag
  *     @arg @ref FLASH_FLAG_CFGBSY FLASH configuration is busy : program or erase setting are used.
  *     @arg @ref FLASH_FLAG_ECCC FLASH one ECC error has been detected and corrected
  *     @arg @ref FLASH_FLAG_ECCD FLASH two ECC errors have been detected
  * @note (*) availability depends on devices
  * @retval The new state of FLASH_FLAG (SET or RESET).
  */
#define __HAL_FLASH_GET_FLAG(__FLAG__)                    (READ_BIT(FLASH->SR,   (__FLAG__)) == (__FLAG__))

/**
  * @brief  Clear the FLASHs pending flags.
  * @param  __FLAG__ specifies the FLASH flags to clear.
  *   This parameter can be any combination of the following values:
  *     @arg @ref FLASH_FLAG_EOP FLASH End of Operation flag
  *     @arg @ref FLASH_FLAG_OPERR FLASH Operation error flag
  *     @arg @ref FLASH_FLAG_PROGERR FLASH Programming error flag
  *     @arg @ref FLASH_FLAG_WRPERR FLASH Write protection error flag
  *     @arg @ref FLASH_FLAG_PGAERR FLASH Programming alignment error flag
  *     @arg @ref FLASH_FLAG_SIZERR FLASH Size error flag
  *     @arg @ref FLASH_FLAG_PGSERR FLASH Programming sequence error flag
  *     @arg @ref FLASH_FLAG_MISERR FLASH Fast programming data miss error flag
  *     @arg @ref FLASH_FLAG_FASTERR FLASH Fast programming error flag
  *     @arg @ref FLASH_FLAG_RDERR FLASH PCROP read  error flag
  *     @arg @ref FLASH_FLAG_OPTVERR FLASH Option validity error flag
  *     @arg @ref FLASH_FLAG_ECCC FLASH one ECC error has been detected and corrected
  *     @arg @ref FLASH_FLAG_ECCD FLASH two ECC errors have been detected
  *     @arg @ref FLASH_FLAG_ALL_ERRORS FLASH All errors flags
  * @retval None
  */
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)        do {  WRITE_REG(FLASH->SR, (__FLAG__)); }\
                                                   } while(0U)
/**
  * @}
  */

/* Include FLASH HAL Extended module */
#include "py32f003_hal_flash_ex.h"
/* Exported variables --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Variables FLASH Exported Variables
  * @{
  */
extern FLASH_ProcessTypeDef pFlash;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup FLASH_Exported_Functions
  * @{
  */

/* Program operation functions  ***********************************************/
/** @addtogroup FLASH_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef  HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint32_t *DataAddr );
HAL_StatusTypeDef  HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
/* FLASH IRQ handler method */
void               HAL_FLASH_IRQHandler(void);
/* Callbacks in non blocking modes */
void               HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void               HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError);
void               HAL_FLASH_Init(uint32_t u32ClockID);
/**
  * @}
  */

/* Peripheral Control functions  **********************************************/
/** @addtogroup FLASH_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef  HAL_FLASH_Unlock(void);
HAL_StatusTypeDef  HAL_FLASH_Lock(void);
/* Option bytes control */
HAL_StatusTypeDef  HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef  HAL_FLASH_OB_Lock(void);
HAL_StatusTypeDef  HAL_FLASH_OB_Launch(void);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);
/**
  * @}
  */

/* Peripheral State functions  ************************************************/
/** @addtogroup FLASH_Exported_Functions_Group3
  * @{
  */
uint32_t HAL_FLASH_GetError(void);
/**
  * @}
  */

/**
  * @}
  */

/* Private types --------------------------------------------------------*/
/** @defgroup FLASH_Private_types FLASH Private Types
  * @{
  */
HAL_StatusTypeDef  FLASH_WaitForLastOperation(uint32_t Timeout);
/**
  * @}
  */

/* Private constants --------------------------------------------------------*/
/** @defgroup FLASH_Private_Constants FLASH Private Constants
  * @{
  */
#define FLASH_SIZE_DATA_REGISTER        FLASHSIZE_BASE

#define FLASH_SIZE                      (((*((uint32_t *)FLASH_SIZE_DATA_REGISTER)) & (0x00FFU)) << 10U)

#define FLASH_BANK_SIZE                 (FLASH_SIZE)   /*!< FLASH Bank Size */

#define FLASH_PAGE_SIZE                 0x00000080U    /*!< FLASH Page Size, 128 Bytes */

#define FLASH_PAGE_NB                   32U

#define FLASH_TIMEOUT_VALUE             1000U          /*!< FLASH Execution Timeout, 1 s */

#define FLASH_TYPENONE                  0x00000000U    /*!< No Programmation Procedure On Going */

#if defined(FLASH_PCROP_SUPPORT)
//#define FLASH_FLAG_SR_ERROR             (FLASH_FLAG_OPERR  | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR |  \
//                                         FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR  | FLASH_FLAG_PGSERR |   \
//                                         FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR |   \
//                                         FLASH_FLAG_OPTVERR)     /*!< All SR error flags */

#define FLASH_FLAG_SR_ERROR             (FLASH_FLAG_OPTVERR  | FLASH_FLAG_WRPERR)     /*!< All SR error flags */
#else
#define FLASH_FLAG_SR_ERROR             (FLASH_FLAG_OPERR  | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR |  \
                                         FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR  | FLASH_FLAG_PGSERR |   \
                                         FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR |   \
                                         FLASH_FLAG_OPTVERR)     /*!< All SR error flags */
#endif

#define FLASH_FLAG_SR_CLEAR             (FLASH_FLAG_SR_ERROR | FLASH_SR_EOP)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup FLASH_Private_Macros FLASH Private Macros
 *  @{
 */
#define IS_FLASH_MAIN_MEM_ADDRESS(__ADDRESS__)         (((__ADDRESS__) >= (FLASH_BASE)) && ((__ADDRESS__) <= (FLASH_BASE + FLASH_SIZE - 1UL)))

#define IS_FLASH_PROGRAM_MAIN_MEM_ADDRESS(__ADDRESS__) (((__ADDRESS__) >= (FLASH_BASE)) && ((__ADDRESS__) <= (FLASH_BASE + FLASH_SIZE - 8UL)))

#define IS_FLASH_PROGRAM_OTP_ADDRESS(__ADDRESS__)      (((__ADDRESS__) >= 0x1FFF7000U) && ((__ADDRESS__) <= (0x1FFF7400U - 8UL)))

#define IS_FLASH_PROGRAM_ADDRESS(__ADDRESS__)          ((IS_FLASH_PROGRAM_MAIN_MEM_ADDRESS(__ADDRESS__)) || (IS_FLASH_PROGRAM_OTP_ADDRESS(__ADDRESS__)))

#define IS_FLASH_FAST_PROGRAM_ADDRESS(__ADDRESS__)     (((__ADDRESS__) >= (FLASH_BASE)) && ((__ADDRESS__) <= (FLASH_BASE + FLASH_SIZE - 256UL)))

#define IS_FLASH_PAGE(__PAGE__)                        ((__PAGE__) < FLASH_PAGE_NB)

#define IS_FLASH_BANK(__BANK__)                        ((__BANK__) == 0x00UL)

#define IS_FLASH_TYPEERASE(__VALUE__)                  (((__VALUE__) == FLASH_TYPEERASE_PAGES) || \
                                                        ((__VALUE__) == FLASH_TYPEERASE_MASS))

#define IS_FLASH_TYPEPROGRAM(__VALUE__)                (((__VALUE__) == FLASH_TYPEPROGRAM_DOUBLEWORD) || \
                                                        ((__VALUE__) == FLASH_TYPEPROGRAM_FAST))

#define IS_FLASH_TIMECONFIG_CLOCK(__VALUE__)           (((__VALUE__) == FLASH_PROGRAM_ERASE_CLOCK_4MHZ) || \
                                                        ((__VALUE__) == FLASH_PROGRAM_ERASE_CLOCK_8MHZ) || \
                                                        ((__VALUE__) == FLASH_PROGRAM_ERASE_CLOCK_16MHZ) || \
                                                        ((__VALUE__) == FLASH_PROGRAM_ERASE_CLOCK_22P12MHZ) || \
                                                        ((__VALUE__) == FLASH_PROGRAM_ERASE_CLOCK_24MHZ))

#define IS_OPTIONBYTE(__VALUE__)                       ((((__VALUE__) & OPTIONBYTE_ALL) != 0x00U) && \
                                                       (((__VALUE__) & ~OPTIONBYTE_ALL) == 0x00U))

#define IS_OB_WRPAREA(__VALUE__)                       (((__VALUE__) == OB_WRPAREA_ZONE_A) || ((__VALUE__) == OB_WRPAREA_ZONE_B))

#define IS_OB_RDP_LEVEL(__LEVEL__)                     (((__LEVEL__) == OB_RDP_LEVEL_0)   ||\
                                                        ((__LEVEL__) == OB_RDP_LEVEL_1)   ||\
                                                        ((__LEVEL__) == OB_RDP_LEVEL_2))

#define IS_OB_USER_TYPE(__TYPE__)                      ((((__TYPE__) & OB_USER_ALL) != 0x00U) && \
                                                        (((__TYPE__) & ~OB_USER_ALL) == 0x00U))

#define IS_OB_USER_CONFIG(__TYPE__,__CONFIG__)         ((~(__TYPE__) & (__CONFIG__)) == 0x00U)

#if defined(FLASH_PCROP_SUPPORT)
#define IS_OB_PCROP_CONFIG(__CONFIG__)                 (((__CONFIG__) & ~(OB_PCROP_ZONE_A | OB_PCROP_ZONE_B | OB_PCROP_RDP_ERASE)) == 0x00U)
#endif

#if defined(FLASH_SECURABLE_MEMORY_SUPPORT)
#define IS_OB_SEC_BOOT_LOCK(__VALUE__)                 (((__VALUE__) == OB_BOOT_ENTRY_FORCED_NONE) || ((__VALUE__) == OB_BOOT_ENTRY_FORCED_FLASH))

#define IS_OB_SEC_SIZE(__VALUE__)                      ((__VALUE__) < (FLASH_PAGE_NB + 1U))
#endif

#define IS_FLASH_LATENCY(__LATENCY__)                  (((__LATENCY__) == FLASH_LATENCY_0) || \
                                                        ((__LATENCY__) == FLASH_LATENCY_1) || \
                                                        ((__LATENCY__) == FLASH_LATENCY_2))
                                                        
#define __HAL_FLASH_TIME_REG_SET(__EPPARA0__,__EPPARA1__,__EPPARA2__,__EPPARA3__,__EPPARA4__)           \
                                                        do {                                            \
                                                            HAL_FLASH_Unlock();                         \
                                                            FLASH->TS0  = (__EPPARA0__)&0xFF;           \
                                                            FLASH->TS1  = ((__EPPARA0__)>>16)&0xFF;     \
                                                            FLASH->TS3  = ((__EPPARA0__)>>8)&0xFF;      \
                                                            FLASH->TS2P = (__EPPARA1__)&0xFF;           \
                                                            FLASH->TPS3 = ((__EPPARA1__)>>16)&0x7FF;    \
                                                            FLASH->PERTPE = (__EPPARA2__)&0x1FFFF;      \
                                                            FLASH->SMERTPE = (__EPPARA3__)&0x1FFFF;     \
                                                            FLASH->PRGTPE = (__EPPARA4__)&0xFFFF;       \
                                                            FLASH->PRETPE = ((__EPPARA4__)>>16)&0x3FFF; \
                                                            HAL_FLASH_Lock();                           \
                                                         } while(0U)

#define IS_WRPSTATE(__VALUE__)          (((__VALUE__) == OB_WRPSTATE_DISABLE) || \
                                         ((__VALUE__) == OB_WRPSTATE_ENABLE))

#define IS_SDKSTATE(__VALUE__)          (((__VALUE__) == OB_SDKSTATE_DISABLE) || \
                                         ((__VALUE__) == OB_SDKSTATE_ENABLE))
                                                         
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F003_HAL_FLASH_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
