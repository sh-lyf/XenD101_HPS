/**
  ******************************************************************************
  * @file    py32_iwdg.c
  * @author  liujinyang@puyasemi.com
  * @brief   iwdg services
  ******************************************************************************
  */
#include "platform.h"

IWDG_HandleTypeDef   IwdgHandle;

void IWDG_Reload(void)
{
	/* Refresh IWDG: reload counter */
	if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
	{
		/* Refresh Error */
		printf("HAL_IWDG_Refresh Failed\r\n");
        /* Initialization Error */
        while(1);
	}
}

void IWDG_Init(void)
{
    /* WWDG Peripheral clock enable */
    __HAL_RCC_WWDG_CLK_ENABLE();
	
    /* 使能LSI时钟 */
    __HAL_RCC_LSI_ENABLE();
    /* 等待直到LSI READY置位 */
    while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == 0U) {;}


    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;//T=1MS
    IwdgHandle.Init.Reload = (1000);              //1ms*1000=1s
    IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;
    if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
    {
		printf("HAL_IWDG_Init Failed\r\n");
        /* Initialization Error */
        while(1);
    }
}

