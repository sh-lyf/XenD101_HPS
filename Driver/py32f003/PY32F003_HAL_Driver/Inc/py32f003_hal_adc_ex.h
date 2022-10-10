/**
  ******************************************************************************
  * @file    py32f003_hal_adc_ex.h
  * @author  MCU Application Team
  * @brief   Header file of ADC HAL Extension module.
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
#ifndef __PY32F003_HAL_ADC_EX_H
#define __PY32F003_HAL_ADC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f003_hal_def.h"  

/** @addtogroup PY32F003_HAL_Driver
  * @{
  */

/** @addtogroup ADCEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/* Exported constants --------------------------------------------------------*/

/** @defgroup ADC_Exported_Constants ADC Exported Constants
  * @{
  */


#define ADC_CCR_ALL     (ADC_CCR_TSEN | ADC_CCR_VREFEN)


/** @defgroup ADC_External_trigger_source_Regular ADC External trigger source Regular
  * @{
  */
/* List of external triggers with generic trigger name, sorted by trigger     */
/* name:                                                                      */

/* External triggers of regular group for ADC1 */
#define ADC_EXTERNALTRIGCONV_T1_TRGO        ADC1_2_EXTERNALTRIG_T1_TRGO
#define ADC_EXTERNALTRIGCONV_T1_CC4         ADC1_2_EXTERNALTRIG_T1_CC4
#define ADC_EXTERNALTRIGCONV_T3_TRGO        ADC1_2_EXTERNALTRIG_T3_TRGO
#define ADC_SOFTWARE_START                  (ADC_CFGR1_EXTSEL + 1U)



/**
  * @}
  */ 


/** @defgroup ADC_channels ADC channels
  * @{
  */
/* Note: Depending on devices, some channels may not be available on package  */
/*       pins. Refer to device datasheet for channels availability.           */
/* Note: Channels are used by bitfields for setting of channel selection      */
/*       (register ADC_CHSELR) and used by number for setting of analog       */
/*       watchdog channel (bits AWDCH in register ADC_CFGR1).                 */
/*       Channels are defined with decimal numbers and converted them to      */
/*       bitfields when needed.                                               */
#define ADC_CHANNEL_0           ( 0x00000000U)
#define ADC_CHANNEL_1           ( 0x00000001U)
#define ADC_CHANNEL_2           ( 0x00000002U)
#define ADC_CHANNEL_3           ( 0x00000003U)
#define ADC_CHANNEL_4           ( 0x00000004U)
#define ADC_CHANNEL_5           ( 0x00000005U)
#define ADC_CHANNEL_6           ( 0x00000006U)
#define ADC_CHANNEL_7           ( 0x00000007U)
#define ADC_CHANNEL_8           ( 0x00000008U)
#define ADC_CHANNEL_9           ( 0x00000009U)
//#define ADC_CHANNEL_10          ( 0x0000000AU)
#define ADC_CHANNEL_11          ( 0x0000000BU)
#define ADC_CHANNEL_12          ( 0x0000000CU)


#define ADC_CHANNEL_TEMPSENSOR  ADC_CHANNEL_11
#define ADC_CHANNEL_VREFINT     ADC_CHANNEL_12
    

/**
  * @}
  */

/**
  * @}
  */
    
/* Exported macro ------------------------------------------------------------*/


/* Private macros ------------------------------------------------------------*/

/** @defgroup ADCEx_Private_Macros ADCEx Private Macros
  * @{
  */
/* Macro reserved for internal HAL driver usage, not intended to be used in   */
/* code of final user.                                                        */

/**
  * @brief Test if the selected ADC channel is an internal channel
  *        VrefInt/TempSensor/Vbat
  *        Note:  
  * @param __CHANNEL__ ADC channel
  * @retval None
  */

#define ADC_IS_CHANNEL_INTERNAL(__CHANNEL__)                                   \
 (((__CHANNEL__) == ADC_CHANNEL_TEMPSENSOR) ||                                 \
  ((__CHANNEL__) == ADC_CHANNEL_VREFINT)                                       \
 )

   
/**
  * @brief Select the internal measurement path to be enabled/disabled 
  *        corresponding to the selected ADC internal channel 
  *        VrefInt/TempSensor/Vbat.
  *        Note: 
  * @param __CHANNEL__ ADC channel
  * @retval Bit of register ADC_CCR
  */

#define ADC_CHANNEL_INTERNAL_PATH(__CHANNEL__)                                 \
 (( (__CHANNEL__) == ADC_CHANNEL_TEMPSENSOR                                    \
  )?                                                                           \
   (ADC_CCR_TSEN)                                                              \
   :                                                                           \
	 (ADC_CCR_VREFEN)                                                            \
 )

#define IS_ADC_EXTTRIG(REGTRIG) (((REGTRIG) == ADC_EXTERNALTRIGCONV_T1_TRGO)  || \
                                 ((REGTRIG) == ADC_EXTERNALTRIGCONV_T1_CC4)   || \
                                 ((REGTRIG) == ADC_EXTERNALTRIGCONV_T3_TRGO)  || \
                                 ((REGTRIG) == ADC_SOFTWARE_START))   



#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) == ADC_CHANNEL_0)           || \
                                 ((CHANNEL) == ADC_CHANNEL_1)           || \
                                 ((CHANNEL) == ADC_CHANNEL_2)           || \
                                 ((CHANNEL) == ADC_CHANNEL_3)           || \
                                 ((CHANNEL) == ADC_CHANNEL_4)           || \
                                 ((CHANNEL) == ADC_CHANNEL_5)           || \
                                 ((CHANNEL) == ADC_CHANNEL_6)           || \
                                 ((CHANNEL) == ADC_CHANNEL_7)           || \
                                 ((CHANNEL) == ADC_CHANNEL_8)           || \
                                 ((CHANNEL) == ADC_CHANNEL_9)           || \
                                 ((CHANNEL) == ADC_CHANNEL_TEMPSENSOR)  || \
                                 ((CHANNEL) == ADC_CHANNEL_VREFINT)     )


/**
  * @}
  */ 

   
/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADCEx_Exported_Functions
  * @{
  */

/* IO operation functions  *****************************************************/
/** @addtogroup ADCEx_Exported_Functions_Group1
  * @{
  */

/* ADC calibration */
HAL_StatusTypeDef       HAL_ADCEx_Calibration_Start(ADC_HandleTypeDef* hadc);
/**
  * @}
  */


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

#endif /* __PY32F003_HAL_ADC_EX_H */


/************************ (C) COPYRIGHT Puya *****END OF FILE****/

