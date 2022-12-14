/**
  ******************************************************************************
  * @file    py32f003_hal_rcc_ex.c
  * @author  MCU Application Team
  * @brief   Extended RCC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities RCC extended peripheral:
  *           + Extended Peripheral Control functions
  *           + Extended Clock management functions
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "py32f003_hal.h"

/** @addtogroup PY32F003_HAL_Driver
  * @{
  */

/** @defgroup RCCEx RCCEx
  * @brief RCC Extended HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup RCCEx_Private_Constants RCCEx Private Constants
 * @{
 */
//#define PLL_TIMEOUT_VALUE        100U /* 100 ms (minimum Tick + 1)  */

#define LSCO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define LSCO_GPIO_PORT        GPIOA
#define LSCO_PIN              GPIO_PIN_2
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup RCCEx_Exported_Functions RCCEx Exported Functions
  * @{
  */

/** @defgroup RCCEx_Exported_Functions_Group1 Extended Peripheral Control functions
 *  @brief  Extended Peripheral Control functions
 *
@verbatim
 ===============================================================================
                ##### Extended Peripheral Control functions  #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the RCC Clocks
    frequencies.
    [..]
    (@) Important note: Care must be taken when @ref HAL_RCCEx_PeriphCLKConfig() is used to
        select the RTC clock source; in this case the Backup domain will be reset in
        order to modify the RTC Clock source, as consequence RTC registers (including
        the backup registers) and RCC_BDCR register are set to their reset values.

@endverbatim
  * @{
  */
/**
  * @brief  Initialize the RCC extended peripherals clocks according to the specified
  *         parameters in the @ref RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit  pointer to a @ref RCC_PeriphCLKInitTypeDef structure that
  *         contains a field PeriphClockSelection which can be a combination of the following values:
  *            @arg @ref RCC_PERIPHCLK_RTC  RTC peripheral clock
  *            @arg @ref RCC_PERIPHCLK_ADC    ADC peripheral clock
  *            @arg @ref RCC_PERIPHCLK_I2C1   I2C1 peripheral clock
  *            @arg @ref RCC_PERIPHCLK_I2S1   I2S1 peripheral clock
  *            @arg @ref RCC_PERIPHCLK_USART1 USART1 peripheral clock
  *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock     (1)
  *            @arg @ref RCC_PERIPHCLK_LPTIM1  LPTIM1 peripheral clock  (1)
  *            @arg @ref RCC_PERIPHCLK_LPTIM2  LPTIM2 peripheral clock  (1)
  *            @arg @ref RCC_PERIPHCLK_LPUART1 LPUART1 peripheral clock (1)
  *            @arg @ref RCC_PERIPHCLK_RNG     RNG peripheral clock     (1)
  *            @arg @ref RCC_PERIPHCLK_TIM1    TIM1 peripheral clock    (1)(2)
  *            @arg @ref RCC_PERIPHCLK_TIM15   TIM15 peripheral clock   (1)(2)
  *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock     (2)
  *
  * @note   (1) Peripherals are not available on all devices
  * @note   (2) Peripherals clock selection is not available on all devices
  * @note   Care must be taken when @ref HAL_RCCEx_PeriphCLKConfig() is used to select
  *         the RTC clock source: in this case the access to Backup domain is enabled.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
    uint32_t tickstart = 0U, temp_reg = 0U;
    FlagStatus       pwrclkchanged = RESET;

    /* Check the parameters */
    assert_param(IS_RCC_PERIPHCLOCK(PeriphClkInit->PeriphClockSelection));

    /*------------------------------- RTC/LCD Configuration ------------------------*/
    if ((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == RCC_PERIPHCLK_RTC))
    {
        /* check for RTC Parameters used to output RTCCLK */
        assert_param(IS_RCC_RTCCLKSOURCE(PeriphClkInit->RTCClockSelection));



        /* As soon as function is called to change RTC clock source, activation of the
           power domain is done. */
        /* Requires to enable write access to Backup Domain of necessary */
        if (__HAL_RCC_PWR_IS_CLK_DISABLED())
        {
            __HAL_RCC_PWR_CLK_ENABLE();
            pwrclkchanged = SET;
        }

        if (HAL_IS_BIT_CLR(PWR->CR1, PWR_CR1_DBP))
        {
            /* Enable write access to Backup domain */
            SET_BIT(PWR->CR1, PWR_CR1_DBP);

            /* Wait for Backup domain Write protection disable */
            tickstart = HAL_GetTick();

            while (HAL_IS_BIT_CLR(PWR->CR1, PWR_CR1_DBP))
            {
                if ((HAL_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
                {
                    return HAL_TIMEOUT;
                }
            }
        }

        /* Reset the Backup domain only if the RTC Clock source selection is modified from reset value */
        temp_reg = (RCC->BDCR & RCC_BDCR_RTCSEL);
        if ((temp_reg != 0x00000000U) && (temp_reg != (PeriphClkInit->RTCClockSelection & RCC_BDCR_RTCSEL)))
        {
            /* Store the content of BDCR register before the reset of Backup Domain */
            temp_reg = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));
            /* RTC Clock selection can be changed only if the Backup Domain is reset */
            __HAL_RCC_BACKUPRESET_FORCE();
            __HAL_RCC_BACKUPRESET_RELEASE();
            /* Restore the Content of BDCR register */
            RCC->BDCR = temp_reg;

            /* Wait for LSERDY if LSE was enabled */
//            if (HAL_IS_BIT_SET(temp_reg, RCC_BDCR_LSEON))
//            {
//                /* Get Start Tick */
//                tickstart = HAL_GetTick();

//                /* Wait till LSE is ready */
//                while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
//                {
//                    if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
//                    {
//                        return HAL_TIMEOUT;
//                    }
//                }
//            }
        }
        __HAL_RCC_RTC_CONFIG(PeriphClkInit->RTCClockSelection);

        /* Require to disable power clock if necessary */
        if (pwrclkchanged == SET)
        {
            __HAL_RCC_PWR_CLK_DISABLE();
        }
    }




#if defined(RCC_CCIPR_USART2SEL)
    /*-------------------------- USART2 clock source configuration -------------------*/
    if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART2) == RCC_PERIPHCLK_USART2)
    {
        /* Check the parameters */
        assert_param(IS_RCC_USART2CLKSOURCE(PeriphClkInit->Usart2ClockSelection));

        /* Configure the USART2 clock source */
        __HAL_RCC_USART2_CONFIG(PeriphClkInit->Usart2ClockSelection);
    }
#endif /* RCC_CCIPR_USART2SEL */

//#if defined(RCC_CCIPR_LPUART1SEL)
//  /*-------------------------- LPUART1 clock source configuration ------------------*/
//  if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPUART1) == RCC_PERIPHCLK_LPUART1)
//  {
//    /* Check the parameters */
//    assert_param(IS_RCC_LPUART1CLKSOURCE(PeriphClkInit->Lpuart1ClockSelection));

//    /* Configure the LPUAR1 clock source */
//    __HAL_RCC_LPUART1_CONFIG(PeriphClkInit->Lpuart1ClockSelection);
//  }
//#endif /* RCC_CCIPR_LPUART1SEL */

#if defined(RCC_CCIPR_LPTIMSEL)
    /*-------------------------- LPTIM1 clock source configuration -------------------*/
    if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPTIM) == (RCC_PERIPHCLK_LPTIM))
    {
        assert_param(IS_RCC_LPTIM1CLKSOURCE(PeriphClkInit->LptimClockSelection));
        __HAL_RCC_LPTIM_CONFIG(PeriphClkInit->LptimClockSelection);
    }
#endif /* RCC_CCIPR_LPTIM1SEL */




//  /*-------------------------- ADC clock source configuration ----------------------*/
//  if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_ADC) == RCC_PERIPHCLK_ADC)
//  {
//    /* Check the parameters */
//    assert_param(IS_RCC_ADCCLKSOURCE(PeriphClkInit->AdcClockSelection));

//    /* Configure the ADC interface clock source */
//    __HAL_RCC_ADC_CONFIG(PeriphClkInit->AdcClockSelection);

//    if (PeriphClkInit->AdcClockSelection == RCC_ADCCLKSOURCE_PLLADC)
//    {
//      /* Enable PLLPCLK output */
//      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLLPCLK);
//    }
//  }



#if defined(RCC_CCIPR_TIM1SEL)
    /*-------------------------- TIM1 clock source configuration ---------------------*/
    if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_TIM1) == RCC_PERIPHCLK_TIM1)
    {
        /* Check the parameters */
        assert_param(IS_RCC_TIM1CLKSOURCE(PeriphClkInit->Tim1ClockSelection));

        /* Configure the TIM1 clock source */
        __HAL_RCC_TIM1_CONFIG(PeriphClkInit->Tim1ClockSelection);

        if (PeriphClkInit->Tim1ClockSelection == RCC_TIM1CLKSOURCE_PLL)
        {
            /* Enable PLLQCLK output */
            __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLLQCLK);
        }
    }
#endif /* RCC_CCIPR_TIM1SEL */





    return HAL_OK;
}

/**
  * @brief  Get the RCC_ClkInitStruct according to the internal RCC configuration registers.
  * @param  PeriphClkInit pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         returns the configuration information for the Extended Peripherals
  *         clocks: I2C1, I2S1, USART1, RTC, ADC,
  *         LPTIM1 (1), LPTIM2 (1), TIM1 (2), TIM15 (1)(2), USART2 (2), LPUART1 (1), CEC (1) and RNG (1)
  * @note (1) Peripheral is not available on all devices
  * @note (2) Peripheral clock selection is not available on all devices
  * @retval None
  */
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
    /* Set all possible values for the extended clock type parameter------------*/
    PeriphClkInit->PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_I2S1 | \
                                          RCC_PERIPHCLK_ADC     | RCC_PERIPHCLK_RTC ;

#if defined(RCC_CCIPR_LPTIM1SEL) && defined(RCC_CCIPR_LPTIM2SEL)
    PeriphClkInit->PeriphClockSelection |=  RCC_PERIPHCLK_LPTIM2  | RCC_PERIPHCLK_LPTIM1;
#endif /* RCC_CCIPR_LPTIM1SEL && RCC_CCIPR_LPTIM2SEL */

#if defined(RCC_CCIPR_LPUART1SEL)
    PeriphClkInit->PeriphClockSelection |=  RCC_PERIPHCLK_LPUART1;
#endif /* RCC_CCIPR_LPUART1SEL */

#if defined(RCC_CCIPR_TIM1SEL)
    PeriphClkInit->PeriphClockSelection |=  RCC_PERIPHCLK_TIM1;
#endif /* RCC_CCIPR_TIM1SEL */

#if defined(RCC_CCIPR_USART2SEL)
    PeriphClkInit->PeriphClockSelection |=  RCC_PERIPHCLK_USART2;
#endif /* RCC_CCIPR_USART2SEL */


#if defined(RCC_CCIPR_USART2SEL)
    /* Get the USART2 clock source ---------------------------------------------*/
    PeriphClkInit->Usart2ClockSelection  = __HAL_RCC_GET_USART2_SOURCE();
#endif /* RCC_CCIPR_USART2SEL */
#if defined(RCC_CCIPR_LPUART1SEL)
    /* Get the LPUART1 clock source --------------------------------------------*/
    PeriphClkInit->Lpuart1ClockSelection = __HAL_RCC_GET_LPUART1_SOURCE();
#endif /* RCC_CCIPR_LPUART1SEL */
    /* Get the I2C1 clock source -----------------------------------------------*/
//  PeriphClkInit->I2c1ClockSelection    = __HAL_RCC_GET_I2C1_SOURCE();
#if defined(RCC_CCIPR_LPTIM1SEL)
    /* Get the LPTIM1 clock source ---------------------------------------------*/
    PeriphClkInit->Lptim1ClockSelection  = __HAL_RCC_GET_LPTIM1_SOURCE();
#endif /* RCC_CCIPR_LPTIM1SEL */
#if defined(RCC_CCIPR_LPTIM2SEL)
    /* Get the LPTIM2 clock source ---------------------------------------------*/
    PeriphClkInit->Lptim2ClockSelection  = __HAL_RCC_GET_LPTIM2_SOURCE();
#endif /* RCC_CCIPR_LPTIM2SEL */
#if defined(RCC_CCIPR_TIM1SEL)
    /* Get the TIM1 clock source ---------------------------------------------*/
    PeriphClkInit->Tim1ClockSelection  = __HAL_RCC_GET_TIM1_SOURCE();
#endif /* RCC_CCIPR_TIM1SEL */

    /* Get the RTC clock source ------------------------------------------------*/
    PeriphClkInit->RTCClockSelection     = __HAL_RCC_GET_RTC_SOURCE();
#if defined(RCC_CCIPR_RNGSEL)
    /* Get the RNG clock source ------------------------------------------------*/
    PeriphClkInit->RngClockSelection     = __HAL_RCC_GET_RNG_SOURCE();
#endif  /* RCC_CCIPR_RNGSEL */
    /* Get the ADC clock source -----------------------------------------------*/
//  PeriphClkInit->AdcClockSelection     = __HAL_RCC_GET_ADC_SOURCE();


}

/**
  * @brief  Return the peripheral clock frequency for peripherals with clock source from PLL
  * @note   Return 0 if peripheral clock identifier not managed by this API
  * @param  PeriphClk  Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_PERIPHCLK_RTC     RTC peripheral clock
  *            @arg @ref RCC_PERIPHCLK_ADC     ADC peripheral clock
  *            @arg @ref RCC_PERIPHCLK_I2C1    I2C1 peripheral clock
  *            @arg @ref RCC_PERIPHCLK_I2S1    I2S1 peripheral clock
  *            @arg @ref RCC_PERIPHCLK_USART1  USART1 peripheral clock
  *            @arg @ref RCC_PERIPHCLK_RNG     RNG peripheral clock    (1)
  *            @arg @ref RCC_PERIPHCLK_TIM15   TIM15 peripheral clock  (1)(2)
  *            @arg @ref RCC_PERIPHCLK_TIM1    TIM1 peripheral clock   (1)(2)
  *            @arg @ref RCC_PERIPHCLK_LPTIM1  LPTIM1 peripheral clock (1)
  *            @arg @ref RCC_PERIPHCLK_LPTIM2  LPTIM2 peripheral clock (1)
  *            @arg @ref RCC_PERIPHCLK_LPUART1 LPUART1 peripheral clock(1)
  *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock    (1)
  *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock (1)(2)
  * @note   (1) Peripheral not available on all devices
  * @note   (2) Peripheral Clock configuration not available on all devices
  * @retval Frequency in Hz
  */
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk)
{
    uint32_t frequency = 0U;
    uint32_t srcclk;
//  uint32_t pllvco;
//  uint32_t plln;
#if defined(RCC_CCIPR_RNGSEL)
    uint32_t rngclk;
    uint32_t rngdiv;
#endif
    /* Check the parameters */
    assert_param(IS_RCC_PERIPHCLOCK(PeriphClk));

    if (PeriphClk == RCC_PERIPHCLK_RTC)
    {
        /* Get the current RTC source */
        srcclk = __HAL_RCC_GET_RTC_SOURCE();

        /* Check if LSE is ready and if RTC clock selection is LSE */
//        if ((HAL_IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)) && (srcclk == RCC_RTCCLKSOURCE_LSE))
//        {
//            frequency = LSE_VALUE;
//        }
        /* Check if LSI is ready and if RTC clock selection is LSI */
        if ((HAL_IS_BIT_SET(RCC->CSR, RCC_CSR_LSIRDY)) && (srcclk == RCC_RTCCLKSOURCE_LSI))
        {
            frequency = LSI_VALUE;
        }
        /* Check if HSE is ready  and if RTC clock selection is HSI_DIV32*/
        else if ((HAL_IS_BIT_SET(RCC->CR, RCC_CR_HSERDY)) &&(srcclk == RCC_RTCCLKSOURCE_HSE_DIV32))
        {
            frequency = HSE_VALUE / 32U;
        }
        /* Clock not enabled for RTC*/
        else
        {
            /* Nothing to do as frequency already initialized to 0U */
        }
    }
    else
    {
        /* Other external peripheral clock source than RTC */

        /* Compute PLL clock input */
//    if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI)  /* HSI ? */
//    {
//      pllvco = HSI_VALUE;
//    }
//    else if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSE)  /* HSE ? */
//    {
//      pllvco = HSE_VALUE;
//    }
//    else /* No source */
//    {
//      pllvco = 0U;
//    }

        /* f(PLL Source) / PLLM */
//    pllvco = (pllvco / ((READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos) + 1U));

        switch (PeriphClk)
        {
#if defined(RCC_CCIPR_USART1)
        case RCC_PERIPHCLK_USART1:
            /* Get the current USART1 source */
            srcclk = __HAL_RCC_GET_USART1_SOURCE();

            if (srcclk == RCC_USART1CLKSOURCE_PCLK1)            /* PCLK1 ? */
            {
                frequency = HAL_RCC_GetPCLK1Freq();
            }
//        else if (srcclk == RCC_USART1CLKSOURCE_SYSCLK)     /* SYSCLK ? */
//        {
//          frequency = HAL_RCC_GetSysClockFreq();
//        }
//        else if ((HAL_IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)) && (srcclk == RCC_USART1CLKSOURCE_HSI))
//        {
//          frequency = HSI_VALUE;
//        }
//        else if ((HAL_IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)) && (srcclk == RCC_USART1CLKSOURCE_LSE))
//        {
//          frequency = LSE_VALUE;
//        }
            /* Clock not enabled for USART1 */
            else
            {
                /* Nothing to do as frequency already initialized to 0U */
            }
            break;
#endif


#if defined(RCC_CCIPR_LPUART1SEL)
        case RCC_PERIPHCLK_LPUART1:
            /* Get the current LPUART1 source */
            srcclk = __HAL_RCC_GET_LPUART1_SOURCE();

            if (srcclk == RCC_LPUART1CLKSOURCE_PCLK1)
            {
                frequency = HAL_RCC_GetPCLK1Freq();
            }
            else if (srcclk == RCC_LPUART1CLKSOURCE_SYSCLK)
            {
                frequency = HAL_RCC_GetSysClockFreq();
            }
            else if ((HAL_IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)) && (srcclk == RCC_LPUART1CLKSOURCE_HSI))
            {
                frequency = HSI_VALUE;
            }
            else if ((HAL_IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)) && (srcclk == RCC_LPUART1CLKSOURCE_LSE))
            {
                frequency = LSE_VALUE;
            }
            /* Clock not enabled for LPUART1 */
            else
            {
                /* Nothing to do as frequency already initialized to 0U */
            }
            break;
#endif /* RCC_CCIPR_LPUART1SEL */

#if defined(RCC_CCIPR_ADC)
        case RCC_PERIPHCLK_ADC:

            srcclk = __HAL_RCC_GET_ADC_SOURCE();

            if (srcclk == RCC_ADCCLKSOURCE_SYSCLK)
            {
                frequency = HAL_RCC_GetSysClockFreq();
            }
            else if (srcclk == RCC_ADCCLKSOURCE_HSI)
            {
                frequency = HSI_VALUE;
            }
            else if (srcclk == RCC_ADCCLKSOURCE_PLLADC)
            {
                if (__HAL_RCC_GET_PLLCLKOUT_CONFIG(RCC_PLLPCLK) != 0U)
                {
                    /* f(PLLP) = f(VCO input) * PLLN / PLLP */
                    plln = READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos;
                    frequency = (pllvco * plln) / ((READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos) + 1U);
                }
            }
            /* Clock not enabled for ADC */
            else
            {
                /* Nothing to do as frequency already initialized to 0U */
            }
            break;
#endif

#if defined(RCC_CCIPR_I2C1)
        case RCC_PERIPHCLK_I2C1:
            /* Get the current I2C1 source */
            srcclk = __HAL_RCC_GET_I2C1_SOURCE();

            if (srcclk == RCC_I2C1CLKSOURCE_PCLK1)
            {
                frequency = HAL_RCC_GetPCLK1Freq();
            }
//        else if (srcclk == RCC_I2C1CLKSOURCE_SYSCLK)
//        {
//          frequency = HAL_RCC_GetSysClockFreq();
//        }
//        else if ((HAL_IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)) && (srcclk == RCC_I2C1CLKSOURCE_HSI))
//        {
//          frequency = HSI_VALUE;
//        }
            /* Clock not enabled for I2C1 */
            else
            {
                /* Nothing to do as frequency already initialized to 0U */
            }
            break;
#endif


#if defined(RCC_CCIPR_LPTIM1SEL)
        case RCC_PERIPHCLK_LPTIM1:
            /* Get the current LPTIM1 source */
            srcclk = __HAL_RCC_GET_LPTIM1_SOURCE();

            if (srcclk == RCC_LPTIM1CLKSOURCE_PCLK1)
            {
                frequency = HAL_RCC_GetPCLK1Freq();
            }
            else if ((HAL_IS_BIT_SET(RCC->CSR, RCC_CSR_LSIRDY)) && (srcclk == RCC_LPTIM1CLKSOURCE_LSI))
            {
                frequency = LSI_VALUE;
            }
            else if ((HAL_IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)) && (srcclk == RCC_LPTIM1CLKSOURCE_HSI))
            {
                frequency = HSI_VALUE;
            }
            else if ((HAL_IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)) && (srcclk == RCC_LPTIM1CLKSOURCE_LSE))
            {
                frequency = LSE_VALUE;
            }
            /* Clock not enabled for LPTIM1 */
            else
            {
                /* Nothing to do as frequency already initialized to 0U */
            }
            break;
#endif /* RCC_CCIPR_LPTIM1SEL */


#if defined(RCC_CCIPR_TIM1SEL)
        case RCC_PERIPHCLK_TIM1:

            srcclk = READ_BIT(RCC->CCIPR, RCC_CCIPR_TIM1SEL);

            if (srcclk == RCC_TIM1CLKSOURCE_PLL) /* PLL ? */
            {
                if (__HAL_RCC_GET_PLLCLKOUT_CONFIG(RCC_PLLQCLK) != 0U)
                {
                    /* f(PLLQ) = f(VCO input) * PLLN / PLLQ */
                    plln = READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos;
                    frequency = (pllvco * plln) / ((READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ) >> RCC_PLLCFGR_PLLQ_Pos) + 1U);
                }
            }
            else if (srcclk == RCC_TIM1CLKSOURCE_PCLK1) /* PCLK1 ? */
            {
                frequency = HAL_RCC_GetPCLK1Freq();
            }
            else /* No clock source */
            {
                /* Nothing to do as frequency already initialized to 0U */
            }
            break;
#endif /* RCC_CCIPR_TIM1SEL */



        default:
            break;
        }
    }

    return (frequency);
}

/**
  * @}
  */

/** @defgroup RCCEx_Exported_Functions_Group2 Extended Clock management functions
 *  @brief  Extended Clock management functions
 *
@verbatim
 ===============================================================================
                ##### Extended clock management functions  #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the
    activation or deactivation of LSE CSS, Low speed clock output and
    clock after wake-up from STOP mode.
@endverbatim
  * @{
  */

///**
//  * @brief  Select the Low Speed clock source to output on LSCO pin (PA2).
//  * @param  LSCOSource  specifies the Low Speed clock source to output.
//  *          This parameter can be one of the following values:
//  *            @arg @ref RCC_LSCOSOURCE_LSI  LSI clock selected as LSCO source
//  *            @arg @ref RCC_LSCOSOURCE_LSE  LSE clock selected as LSCO source
//  * @retval None
//  */
//void HAL_RCCEx_EnableLSCO(uint32_t LSCOSource)
//{
//  GPIO_InitTypeDef GPIO_InitStruct;
//  FlagStatus       pwrclkchanged = RESET;
//  FlagStatus       backupchanged = RESET;

//  /* Check the parameters */
//  assert_param(IS_RCC_LSCOSOURCE(LSCOSource));

//  /* LSCO Pin Clock Enable */
//  LSCO_CLK_ENABLE();

//  /* Configue the LSCO pin in analog mode */
//  GPIO_InitStruct.Pin = LSCO_PIN;
//  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(LSCO_GPIO_PORT, &GPIO_InitStruct);

//  /* Update LSCOSEL clock source in Backup Domain control register */
//  if (__HAL_RCC_PWR_IS_CLK_DISABLED())
//  {
//    __HAL_RCC_PWR_CLK_ENABLE();
//    pwrclkchanged = SET;
//  }
//  if (HAL_IS_BIT_CLR(PWR->CR1, PWR_CR1_DBP))
//  {
//    HAL_PWR_EnableBkUpAccess();
//    backupchanged = SET;
//  }

//  MODIFY_REG(RCC->BDCR, RCC_BDCR_LSCOSEL | RCC_BDCR_LSCOEN, LSCOSource | RCC_BDCR_LSCOEN);

//  if (backupchanged == SET)
//  {
//    HAL_PWR_DisableBkUpAccess();
//  }
//  if (pwrclkchanged == SET)
//  {
//    __HAL_RCC_PWR_CLK_DISABLE();
//  }
//}

///**
//  * @brief  Disable the Low Speed clock output.
//  * @retval None
//  */
//void HAL_RCCEx_DisableLSCO(void)
//{
//  FlagStatus       pwrclkchanged = RESET;
//  FlagStatus       backupchanged = RESET;

//  /* Update LSCOEN bit in Backup Domain control register */
//  if (__HAL_RCC_PWR_IS_CLK_DISABLED())
//  {
//    __HAL_RCC_PWR_CLK_ENABLE();
//    pwrclkchanged = SET;
//  }
//  if (HAL_IS_BIT_CLR(PWR->CR1, PWR_CR1_DBP))
//  {
//    /* Enable access to the backup domain */
//    HAL_PWR_EnableBkUpAccess();
//    backupchanged = SET;
//  }

//  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSCOEN);

//  /* Restore previous configuration */
//  if (backupchanged == SET)
//  {
//    /* Disable access to the backup domain */
//    HAL_PWR_DisableBkUpAccess();
//  }
//  if (pwrclkchanged == SET)
//  {
//    __HAL_RCC_PWR_CLK_DISABLE();
//  }
//}

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_RCC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
