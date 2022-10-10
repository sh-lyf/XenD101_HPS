/**
  ******************************************************************************
  * @file    py32f003_hal_flash.c
  * @author  MCU Application Team
  * @brief   FLASH HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the internal FLASH memory:
  *           + Program operations functions
  *           + Memory Control functions
  *           + Peripheral Errors functions
  *
 @verbatim

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "py32f003_hal.h"

/** @addtogroup PY32F003_HAL_Driver
  * @{
  */

/** @defgroup FLASH FLASH
  * @brief FLASH HAL module driver
  * @{
  */

#ifdef HAL_FLASH_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Variables FLASH Private Variables
 * @{
 */
/**
  * @brief  Variable used for Program/Erase sectors under interruption
  */
FLASH_ProcessTypeDef pFlash  = {.Lock = HAL_UNLOCKED, \
                                .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                .ProcedureOnGoing = FLASH_TYPENONE, \
                                .Address = 0U, \
                                .Page = 0U, \
                                .NbPagesToErase = 0U
                               };
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup FLASH_Private_Functions FLASH Private Functions
 * @{
 */
//static  void  FLASH_Program_Word(uint32_t Address, uint32_t Data);
//static	void 	FLASH_Program(uint32_t Address, uint32_t *pdata);
static 	void 	FLASH_PageErase(uint32_t PageAddress);
static 	void 	FLASH_MassErase(void);
static  void  FLASH_Program_Page(uint32_t Address, uint32_t * DataAddress);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Functions FLASH Exported Functions
  * @{
  */

/** @defgroup FLASH_Exported_Functions_Group1 Programming operation functions
 *  @brief   Programming operation functions
 *
@verbatim
 ===============================================================================
                  ##### Programming operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the FLASH
    program operations.

@endverbatim
  * @{
  */

///**
//  * @brief  Program double word or fast program of a row at a specified address.
//  * @param  TypeProgram Indicate the way to program at a specified address.
//  *                      This parameter can be a value of @ref FLASH_Type_Program
//  * @param  Address Specifies the address to be programmed.
//  * @param  adr:Page Start Address
//  *         sz: Page Size (128bytes)
//  *         buf:Page Data
//  *
//  * @retval HAL_StatusTypeDef HAL Status
//  */
//HAL_StatusTypeDef HAL_FLASH_ProgramPage(unsigned long adr,unsigned long sz, unsigned char *buf)
//{
//
//
//	uint8_t *pdata,*temp_buf;
//	unsigned long start_addr,end_addr;
//  HAL_StatusTypeDef status;

//  /* Check the parameters */
//  assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));
//  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

//  /* Process Locked */
//  __HAL_LOCK(&pFlash);

//  /* Reset error code */
//  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

//  /* Wait for last operation to be completed */
//  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

//  if (status == HAL_OK)
//  {
//		   start_addr=adr&0xffffff80;   //sz first page addr
//		   temp_buf =buf-(adr-start_addr);
//		   pdata=temp_buf;
//		   for(uint8_t i=0;i<adr-start_addr;i++)
//		   {
//				   *temp_buf++=*(__IO uint8_t *)(start_addr+i);
//
//			 }
//			 temp_buf =buf+sz;
//
//		   end_addr=(adr+sz+0x80)&0xffffff80;  //sz last page addr
//			 for(uint8_t i=0;i<end_addr;i++)
//			 {
//				  *temp_buf++=*(__IO uint8_t *)(adr+sz+i);
//			 }
//
//		   while(start_addr>end_addr)
//			 {
//				   FLASH_PageErase(start_addr);
////				   FLASH_Program_Page(start_addr, *((uint32_t *)pdata));
//				   start_addr+=0x80;
//				   pdata+=128;
//
//			 }
//
//
//

//  }

//  /* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);

//  /* return status */
//  return status;
//}

/**
  * @brief  Program double word or fast program of a row at a specified address with interrupt enabled.
  * @param  TypeProgram Indicate the way to program at a specified address.
  *                      This parameter can be a value of @ref FLASH_Type_Program
  * @param  Address Specifies the address to be programmed.
  * @param  Data Specifies the data to be programmed
  *               This parameter is the data for the double word program and the address where
  *               are stored the data for the row fast program.
  *
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
    HAL_StatusTypeDef status;

//  /* Check the parameters */
//  assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));
//  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

//  /* Process Locked */
//  __HAL_LOCK(&pFlash);

//  /* Reset error code */
//  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

//  /* Wait for last operation to be completed */
//  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

//  if (status != HAL_OK)
//  {
//    /* Process Unlocked */
//    __HAL_UNLOCK(&pFlash);
//  }
//  else
//  {
//    /* Set internal variables used by the IRQ handler */
//    pFlash.ProcedureOnGoing = TypeProgram;
//    pFlash.Address = Address;

//    /* Enable End of Operation and Error interrupts */
//    __HAL_FLASH_ENABLE_IT(FLASH_IT_EOP | FLASH_IT_OPERR | FLASH_IT_ECCC);

//    if (TypeProgram == FLASH_TYPEPROGRAM_DOUBLEWORD)
//    {
//      /* Check the parameters */
//      assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

//      /* Program double-word (64-bit) at a specified address */
//      FLASH_Program_DoubleWord(Address, Data);
//    }
//    else
//    {
//      /* Check the parameters */
//      assert_param(IS_FLASH_FAST_PROGRAM_ADDRESS(Address));

//      /* Fast program a 32 row double-word (64-bit) at a specified address */
//      FLASH_Program_Fast(Address, (uint32_t)Data);
//    }
//  }

//  /* return status */
    return status;
}

/**
  * @brief Handle FLASH interrupt request.
  * @retval None
  */
void HAL_FLASH_IRQHandler(void)
{
//  uint32_t param = 0xFFFFFFFFU;
//  uint32_t error;

//  /* Save flash errors. Only ECC detection can be checked here as ECCC
//     generates NMI */
//  error = (FLASH->SR & FLASH_FLAG_SR_ERROR);
//  error |= (FLASH->ECCR & FLASH_FLAG_ECCC);

//  CLEAR_BIT(FLASH->CR, pFlash.ProcedureOnGoing);

//  /* A] Set parameter for user or error callbacks */
//  /* check operation was a program or erase */
//  if ((pFlash.ProcedureOnGoing & (FLASH_TYPEPROGRAM_DOUBLEWORD | FLASH_TYPEPROGRAM_FAST)) != 0x00U)
//  {
//    /* return adress being programmed */
//    param = pFlash.Address;
//  }
//  else if ((pFlash.ProcedureOnGoing & (FLASH_TYPEERASE_MASS | FLASH_TYPEERASE_PAGES)) != 0x00U)
//  {
//    /* return page number being erased (0 for mass erase) */
//    param = pFlash.Page;
//  }
//  else
//  {
//    /* Nothing to do */
//  }

//  /* B] Check errors */
//  if (error != 0x00U)
//  {
//    /*Save the error code*/
//    pFlash.ErrorCode |= error;

//    /* clear error flags */
//    __HAL_FLASH_CLEAR_FLAG(error);

//    /*Stop the procedure ongoing*/
//    pFlash.ProcedureOnGoing = FLASH_TYPENONE;

//    /* Error callback */
//    HAL_FLASH_OperationErrorCallback(param);
//  }

//  /* C] Check FLASH End of Operation flag */
//  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) != 0x00U)
//  {
//    /* Clear FLASH End of Operation pending bit */
//    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);

//    if (pFlash.ProcedureOnGoing == FLASH_TYPEERASE_PAGES)
//    {
//      /* Nb of pages to erased can be decreased */
//      pFlash.NbPagesToErase--;

//      /* Check if there are still pages to erase*/
//      if (pFlash.NbPagesToErase != 0x00U)
//      {
//        /* Increment page number */
//        pFlash.Page++;
//        FLASH_PageErase(pFlash.Page);
//      }
//      else
//      {
//        /* No more pages to erase: stop erase pages procedure */
//        pFlash.ProcedureOnGoing = FLASH_TYPENONE;
//      }
//    }
//    else
//    {
//      /*Stop the ongoing procedure */
//      pFlash.ProcedureOnGoing = FLASH_TYPENONE;
//    }

//    /* User callback */
//    HAL_FLASH_EndOfOperationCallback(param);
//  }

//  if (pFlash.ProcedureOnGoing == FLASH_TYPENONE)
//  {
//    /* Disable End of Operation and Error interrupts */
//    __HAL_FLASH_DISABLE_IT(FLASH_IT_EOP | FLASH_IT_OPERR | FLASH_IT_ECCC);

//    /* Process Unlocked */
//    __HAL_UNLOCK(&pFlash);
//  }
}

/**
  * @brief  FLASH end of operation interrupt callback.
  * @param  ReturnValue The value saved in this parameter depends on the ongoing procedure
  *                  Mass Erase: 0
  *                  Page Erase: Page which has been erased
  *                  Program: Address which was selected for data program
  * @retval None
  */
__weak void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(ReturnValue);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_FLASH_EndOfOperationCallback could be implemented in the user file
     */
}

/**
  * @brief  FLASH operation error interrupt callback.
  * @param  ReturnValue The value saved in this parameter depends on the ongoing procedure
  *                 Mass Erase: 0
  *                 Page Erase: Page number which returned an error
  *                 Program: Address which was selected for data program
  * @retval None
  */
__weak void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(ReturnValue);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_FLASH_OperationErrorCallback could be implemented in the user file
     */
}

/**
  * @}
  */

/** @defgroup FLASH_Exported_Functions_Group2 Peripheral Control functions
 *  @brief   Management functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the FLASH
    memory operations.

@endverbatim
  * @{
  */

/**
  * @brief  Unlock the FLASH control register access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Unlock(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0x00U)
    {
        /* Authorize the FLASH Registers access */
        WRITE_REG(FLASH->KEYR, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR, FLASH_KEY2);

        /* verify Flash is unlock */
        if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0x00U)
        {
            status = HAL_ERROR;
        }
    }

    return status;
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    /* Set the LOCK Bit to lock the FLASH Registers access */
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    /* verify Flash is locked */
    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0x00u)
    {
        status = HAL_OK;
    }

    return status;
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if (READ_BIT(FLASH->CR, FLASH_CR_OPTLOCK) != 0x00U)
    {
        /* Authorizes the Option Byte register programming */
        WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY1);
        WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY2);

        /* verify option bytes are unlocked */
        if (READ_BIT(FLASH->CR, FLASH_CR_OPTLOCK) == 0x00U)
        {
            status = HAL_OK;
        }
    }

    return status;
}

/**
  * @brief  Lock the FLASH Option Bytes Registers access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    /* Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access */
    SET_BIT(FLASH->CR, FLASH_CR_OPTLOCK);

    /* verify option bytes are locked */
    if (READ_BIT(FLASH->CR, FLASH_CR_OPTLOCK) != 0x00u)
    {
        status = HAL_OK;
    }

    return status;
}

/**
  * @brief  Launch the option byte loading.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void)
{
    /* Set the bit to force the option byte reloading */
    SET_BIT(FLASH->CR, FLASH_CR_OBL_LAUNCH);

    /* We should not reach here : Option byte launch generates Option byte reset
       so return error */
    return HAL_ERROR;
}

/**
  * @}
  */

/** @defgroup FLASH_Exported_Functions_Group3 Peripheral State and Errors functions
 *  @brief   Peripheral Errors functions
 *
@verbatim
 ===============================================================================
                ##### Peripheral Errors functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time Errors of the FLASH peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Get the specific FLASH error flag.
  * @retval FLASH_ErrorCode The returned value can be
  *            @arg @ref HAL_FLASH_ERROR_NONE No error set
  *            @arg @ref HAL_FLASH_ERROR_OP FLASH Operation error
  *            @arg @ref HAL_FLASH_ERROR_PROG FLASH Programming error
  *            @arg @ref HAL_FLASH_ERROR_WRP FLASH Write protection error
  *            @arg @ref HAL_FLASH_ERROR_PGA FLASH Programming alignment error
  *            @arg @ref HAL_FLASH_ERROR_SIZ FLASH Size error
  *            @arg @ref HAL_FLASH_ERROR_PGS FLASH Programming sequence error
  *            @arg @ref HAL_FLASH_ERROR_MIS FLASH Fast programming data miss error
  *            @arg @ref HAL_FLASH_ERROR_FAST FLASH Fast programming error
  *            @arg @ref HAL_FLASH_ERROR_RD FLASH Read Protection error (PCROP)(*)
  *            @arg @ref HAL_FLASH_ERROR_OPTV FLASH Option validity error
  *            @arg @ref HAL_FLASH_ERROR_ECCD FLASH two ECC errors have been detected
  * @note (*) availability depends on devices
  */
uint32_t HAL_FLASH_GetError(void)
{
    return pFlash.ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @addtogroup FLASH_Private_Functions
  * @{
  */

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout)
{
//  uint32_t error;
//  uint32_t eccerr;
    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
       Even if the FLASH operation fails, the BUSY flag will be reset and an error
       flag will be set */
    uint32_t timeout = HAL_GetTick() + Timeout;

    /* Wait if any operation is ongoing */
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != 0x00U)
    {
        if (HAL_GetTick() >= timeout)
        {
            return HAL_TIMEOUT;
        }
    }

    /* check flash errors. Only ECC correction can be checked here as ECCD
        generates NMI */
//  error = (FLASH->SR & FLASH_FLAG_SR_ERROR);

    /* Clear SR register */
    FLASH->SR = FLASH_FLAG_SR_CLEAR;


    return HAL_OK;
}
/**
  * @brief  Full erase of FLASH memory Bank
  *
  * @retval None
  */
static void FLASH_MassErase(void)
{
    /* Clean the error context */
    pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
    HAL_FLASH_Unlock();
    /* Only bank1 will be erased*/
    SET_BIT(FLASH->CR, FLASH_CR_MER);
    *(__IO uint32_t *)(0x08000000)=0x12344321;

}

static 	void 	FLASH_PageErase(uint32_t PageAddress)
{
    /* Clean the error context */
    pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
    HAL_FLASH_Unlock();
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    *(__IO uint32_t *)(PageAddress)=0xff;
    while (((FLASH->SR & FLASH_SR_BSY) != 0x00U)&&((FLASH->SR & FLASH_SR_EOP) == 0x00U))
    {
    }


}
static 	void 	FLASH_SectorErase(uint32_t SectorAddress)
{
    HAL_FLASH_Unlock();
    SET_BIT(FLASH->CR, FLASH_CR_SER);
    *(__IO uint32_t *)(SectorAddress)=0xff;
    while (((FLASH->SR & FLASH_SR_BSY) != 0x00U)&&((FLASH->SR & FLASH_SR_EOP) == 0x00U))
    {
    }
}
static __RAM_FUNC void FLASH_Program_Page(uint32_t Address, uint32_t * DataAddress)
{

    uint8_t index=0;
    uint32_t dest = Address;
    uint32_t * src = DataAddress;
    uint32_t primask_bit;



    SET_BIT(FLASH->CR, FLASH_CR_PG);
    /* Enter critical section: row programming should not be longer than 7 ms */
    primask_bit = __get_PRIMASK();
    __disable_irq();
    /* 32 words*/
    while(index<32U)
    {
        *(uint32_t *)dest = *src;
        src += 1U;
        dest += 4U;
        index++;
        if(index==31)
        {
            SET_BIT(FLASH->CR, FLASH_CR_PGSTRT);
        }
    }
    /* wait for BSY1 in order to be sure that flash operation is ended befoire
       allowing prefetch in flash. Timeout does not return status, as it will
       be anyway done later */
    while ((FLASH->SR & FLASH_SR_BSY) != 0x00U)
    {
    }

    /* Exit critical section: restore previous priority mask */
    __set_PRIMASK(primask_bit);
}
///**
//  * @brief  Program double-word (64-bit) at a specified address.
//  * @param  Address Specifies the address to be programmed.
//  * @param  Data Specifies the data to be programmed.
//  * @retval None
//  */

//static void FLASH_Program_Word(uint32_t Address, uint32_t Data)
//{
//  /* Set PG bit */
//
//  SET_BIT(FLASH->CR, FLASH_CR_PG);

//  /* Program first word */
//  *(uint32_t *)Address = (uint32_t)Data;

//  /* Barrier to ensure programming is performed in 2 steps, in right order
//    (independently of compiler optimization behavior) */
//  __ISB();
//}

/**
  * @brief  Fast program a 32 row double-word (64-bit) at a specified address.
  * @param  Address Specifies the address to be programmed.
  * @param  DataAddress Specifies the address where the data are stored.
  * @retval None
  */

/**
  * @}
  */
/**
  * @brief  Perform a mass erase or erase the specified FLASH memory pages
  * @note   To correctly run this function, the @ref HAL_FLASH_Unlock() function
  *         must be called before.
  *         Call the @ref HAL_FLASH_Lock() to disable the flash memory access
  *         (recommended to protect the FLASH memory against possible unwanted operation)
  * @param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  *
  * @param[out]  PageError pointer to variable  that
  *         contains the configuration information on faulty page in case of error
  *         (0xFFFFFFFF means that all the pages have been correctly erased)
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    uint32_t address = 0U;

    /* Process Locked */
    __HAL_LOCK(&pFlash);

    /* Check the parameters */
    assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));

    if (pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
        /* Mass Erase requested for Bank1 */
        /* Wait for last operation to be completed */
        if (FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE) == HAL_OK)
        {
            /*Mass erase to be done*/
            FLASH_MassErase();

            /* Wait for last operation to be completed */
            status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

            /* If the erase operation is completed, disable the MER Bit */
            HAL_FLASH_Unlock();
            CLEAR_BIT(FLASH->CR, FLASH_CR_MER);
//				HAL_FLASH_Lock();
        }
    }
    else if(pEraseInit->TypeErase == FLASH_TYPEERASE_PAGEERASE)
    {
        /* Page Erase is requested */
        /* Check the parameters */
        assert_param(IS_FLASH_PROGRAM_ADDRESS(pEraseInit->PageAddress));
        assert_param(IS_FLASH_NB_PAGES(pEraseInit->PageAddress, pEraseInit->NbPages));

        /* Page Erase requested on address located on bank1 */
        /* Wait for last operation to be completed */
        if (FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE) == HAL_OK)
        {
            /*Initialization of PageError variable*/
            *PageError = 0xFFFFFFFFU;

            /* Erase page by page to be done*/
            for(address = pEraseInit->PageAddress;
                    address < ((pEraseInit->NbPages * FLASH_PAGE_SIZE) + pEraseInit->PageAddress);
                    address += FLASH_PAGE_SIZE)
            {
                FLASH_PageErase(address);

                /* Wait for last operation to be completed */
                status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
                HAL_FLASH_Unlock();
                /* If the erase operation is completed, disable the PER Bit */
                CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
//         HAL_FLASH_Lock();
                if (status != HAL_OK)
                {
                    /* In case of error, stop erase procedure and return the faulty address */
                    *PageError = address;
                    break;
                }
            }
        }
    }
    else if(pEraseInit->TypeErase == FLASH_TYPEERASE_SECTORERASE)
    {
        if (FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE) == HAL_OK)
        {
            FLASH_SectorErase(pEraseInit->SectorAddress);
            /* Wait for last operation to be completed */
            status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

            /* If the erase operation is completed, disable the MER Bit */
            HAL_FLASH_Unlock();
            CLEAR_BIT(FLASH->CR, FLASH_CR_SER);
//				   HAL_FLASH_Lock();
        }
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);

    return status;
}

/**
  * @brief   FLASH Program and Erase Clock Config.
  * @@param  u32ClockID clock select
  *            @arg @ref FLASH_PROGRAM_ERASE_CLOCK_4MHZ
  *            @arg @ref FLASH_PROGRAM_ERASE_CLOCK_8MHZ
  *            @arg @ref FLASH_PROGRAM_ERASE_CLOCK_16MHZ
  *            @arg @ref FLASH_PROGRAM_ERASE_CLOCK_22P12MHZ
  *            @arg @ref FLASH_PROGRAM_ERASE_CLOCK_24MHZ
  * @retval  None
  */
void HAL_FLASH_Init(uint32_t u32ClockID)
{
    //flash擦写时间配置寄存器4MHz/8MHz/16MHz/22.12MHz/24MHz对应的首个寄存器地址
    const uint32_t _aTime[5] = {0x1FFF0F1C, 0x1FFF0F30, 0x1FFF0F44, 0x1FFF0F58, 0x1FFF0F6C};

    assert_param(IS_FLASH_TIMECONFIG_CLOCK(u32ClockID));

    __HAL_FLASH_TIME_REG_SET((*(uint32_t *)(_aTime[u32ClockID])),     \
                             (*(uint32_t *)(_aTime[u32ClockID]+4)),   \
                             (*(uint32_t *)(_aTime[u32ClockID]+8)),   \
                             (*(uint32_t *)(_aTime[u32ClockID]+12)),  \
                             (*(uint32_t *)(_aTime[u32ClockID]+16)));
}

HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint32_t * DataAddr )
{
    HAL_StatusTypeDef status = HAL_ERROR;
//  uint8_t index = 0U;
//  uint8_t nbiterations = 0U;

    /* Process Locked */
    __HAL_LOCK(&pFlash);

    /* Check the parameters */
    assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));
    assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

    if(status == HAL_OK)
    {
        HAL_FLASH_Unlock();

        FLASH_Program_Page(Address, DataAddr);


        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
        HAL_FLASH_Unlock();
        /* If the program operation is completed, disable the PG Bit */
        CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
//		  HAL_FLASH_Lock();
        /* In case of error, stop programming procedure */
        if (status != HAL_OK)
        {
            return status;
        }

    }

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);

    return status;
}
/**
  * @brief  Set the read protection level.
  * @param  ReadProtectLevel specifies the read protection level.
  *         This parameter can be one of the following values:
  *            @arg @ref OB_RDP_LEVEL_0 No protection
  *            @arg @ref OB_RDP_LEVEL_1 Read protection of the memory
  *
  * @note   Warning: When enabling OB_RDP level 2 it's no more possible to go back to level 1 or 0
  * @retval HAL status
  */
//static HAL_StatusTypeDef FLASH_OB_RDP_LevelConfig(uint8_t ReadProtectLevel)
//{
//    HAL_StatusTypeDef status = HAL_OK;

//    /* Check the parameters */
//    assert_param(IS_OB_RDP_LEVEL(ReadProtectLevel));

//    /* Wait for last operation to be completed */
//    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

//    if(status == HAL_OK)
//    {
//        /* Clean the error context */
//        pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

//        /* If the previous operation is completed, proceed to erase the option bytes */

//        HAL_FLASH_Unlock();

//        HAL_FLASH_OB_Unlock();




//        SET_BIT(FLASH->OPTR, ReadProtectLevel);

//        /* Wait for last operation to be completed */
//        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

//    }

//    return status;
//}


/**
  * @brief   Get the Option byte configuration
  * @param  pOBInit pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  * 
  * @retval None
  */
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit)
{
    pOBInit->OptionType = OPTIONBYTE_ALL;
   
    /* Get WRP sector */
    pOBInit->WRPState = OB_WRPSTATE_ENABLE;
    pOBInit->WRPSector = FLASH->WRPR;
   
    /* Get SDK sector */
    pOBInit->SDKStartAddr = (FLASH->SDKR)&0x1F;
    pOBInit->SDKEndAddr = ((FLASH->SDKR)&0x1F00)>>8;
   
    /*Get RDP Level*/
    if (((FLASH->OPTR)&0xFF) == OB_RDP_LEVEL_0)
    {
        pOBInit->RDPLevel   = OB_RDP_LEVEL_0;
    }else
    {
        pOBInit->RDPLevel   = OB_RDP_LEVEL_1;
    }    
   
    /*Get USER*/
    pOBInit->USERConfig = (FLASH->OPTR)&(FLASH_OPTR_WWDG_SW|FLASH_OPTR_IWDG_SW|FLASH_OPTR_NRST_MODE);
   
    /*Get BOR Level*/
    pOBInit->BORLevel   = (FLASH->OPTR)&(FLASH_OPTR_BOR_EN|FLASH_OPTR_BOR_LEV);
   
    /* Get BOOT bit 1 config OB */
    pOBInit->BOOTBit1Config = (FLASH->OPTR)&(FLASH_OPTR_nBOOT1);
}

/**
  * @brief  Program option bytes
  * @note   The function @ref HAL_FLASH_Unlock() should be called before to unlock the FLASH interface
  *         The function @ref HAL_FLASH_OB_Unlock() should be called before to unlock the options bytes
  *         The function @ref HAL_FLASH_OB_Launch() should be called after to force the reload of the options bytes
  *         (system reset will occur)
  *
  * @param  pOBInit pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    /* Process Locked */
    __HAL_LOCK(&pFlash);

    /* Check the parameters */
    assert_param(IS_OPTIONBYTE(pOBInit->OptionType));

    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    /* Write protection configuration */
    if((pOBInit->OptionType & OPTIONBYTE_WRP) == OPTIONBYTE_WRP)
    {
        assert_param(IS_WRPSTATE(pOBInit->WRPState));
        if (pOBInit->WRPState == OB_WRPSTATE_ENABLE)
        {
            /* Enable of Write protection on the selected sector */
            FLASH->WRPR = pOBInit->WRPSector;
        }
        else
        {
            /* Disable of Write protection on the selected page */
            FLASH->WRPR = 0;
        }
    }

    /* SDK protection configuration */
    if((pOBInit->OptionType & OPTIONBYTE_SDK) == OPTIONBYTE_SDK)
    {
        assert_param(IS_SDKSTATE(pOBInit->SDKState));
        if (pOBInit->WRPState == OB_WRPSTATE_ENABLE)
        {
            /* Enable of Write protection on the selected sector */
            FLASH->SDKR = (pOBInit->SDKStartAddr) | (pOBInit->SDKEndAddr<<8);
        }
        else
        {
            /* Disable of Write protection on the selected page */
            FLASH->WRPR = 0;
        }
        
    }

    /* Read protection configuration */
    if((pOBInit->OptionType & OPTIONBYTE_RDP) == OPTIONBYTE_RDP)
    {
        MODIFY_REG(FLASH->OPTR, FLASH_OPTR_RDP, pOBInit->RDPLevel);
    }

    /* USER configuration */
    if((pOBInit->OptionType & OPTIONBYTE_USER) == OPTIONBYTE_USER)
    {
        MODIFY_REG(FLASH->OPTR, (FLASH_OPTR_IWDG_SW|FLASH_OPTR_WWDG_SW|   \
                                 FLASH_OPTR_NRST_MODE),pOBInit->USERConfig);
    }

    /* BOR configuration*/
    if((pOBInit->OptionType & OPTIONBYTE_BOR) == OPTIONBYTE_BOR)
    {
        MODIFY_REG(FLASH->OPTR, (FLASH_OPTR_BOR_EN|FLASH_OPTR_BOR_LEV), \
                                 pOBInit->BORLevel);
    }

    /* BOOTBit1 configuration*/
    if((pOBInit->OptionType & OPTIONBYTE_BOOT_BIT1) == OPTIONBYTE_BOOT_BIT1)
    {
        MODIFY_REG(FLASH->OPTR, (FLASH_OPTR_nBOOT1), pOBInit->BOOTBit1Config);
    }
    
    /* 启动对Flash option bytes的修改 */
    FLASH->CR|=FLASH_CR_OPTSTRT;
    /* EOPIE置位后EOP状态才会置位 */
    FLASH->CR|=FLASH_CR_EOPIE;
    /* 触发正式的写操作 */
    *((__IO uint32_t *)(0x40022080))=0xff;
    /* 等待BSY位被清零 */
    while(FLASH->SR & FLASH_SR_BSY){;}    
    /* 等待EOP拉高，软件清零 */
	while((FLASH->SR & FLASH_SR_EOP)==0){;}
    FLASH->SR = FLASH_SR_EOP;

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);

    HAL_FLASH_Lock();
	HAL_FLASH_OB_Lock();
    
    return status;
}
#endif /* HAL_FLASH_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Puya Semiconductor *****END OF FILE****/
