/**
  ******************************************************************************
  * @file    system_py32f0xx.c
  * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer Header File for
	*					 PY32F030xx Device Series
	*
  ******************************************************************************
  * @attention
  *
  *		Copyright (c) 2012 ARM LIMITED
  *
  *		All rights reserved.
  *
  *		Redistribution and use in source and binary forms, with or without modification,
  *	are permitted provided that the following conditions are met:
	*
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of the copyright holder nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* This file refers the CMSIS standard, some adjustments are made according to Puya chips */

/** @addtogroup PY32F030xx_System_Private_Includes
  * @{
  */
#include "py32f0xx.h"

/** @addtogroup PY32F030xx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE) 
#define HSE_VALUE    8000000U    /*!< Value of the External oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE  8000000U   /*!< Value of the Internal oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSI_VALUE */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x100. */

/** @addtogroup PY32F030xx_System_Private_Variables
  * @{
  */
/* This variable is updated in three ways:
		1) by calling CMSIS function SystemCoreClockUpdate()
		2) by calling HAL API function HAL_RCC_GetHCLKFreq()
		3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
			 Note: If you use this function to configure the system clock; then there
						 is no need to call the 2 first functions listed above, since SystemCoreClock
						 variable is updated automatically.
*/
uint32_t SystemCoreClock = 8000000U;

const uint32_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint32_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

/** @addtogroup PY32F030xx_System_Private_Functions
  * @{
  */

/**
	* @brief  Setup the microcontroller system.
	* @param  none
	* @retval none
	*/
void SystemInit(void)
{
  /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif
}

/**
	* @brief  Update SystemCoreClock variable according to Clock Register Values.
	*         The SystemCoreClock variable contains the core clock (HCLK), it can
	*         be used by the user application to setup the SysTick timer or configure
	*         other parameters.
	* @param  none
	* @retval none
	*/
void SystemCoreClockUpdate(void)
{ 
}
