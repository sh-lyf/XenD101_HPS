/**
  ******************************************************************************
  * @file    utilities.c
  * @author  ting.gao@iclegend.com
  * @brief   project utilities, tools
  ******************************************************************************
  */
#include <stdio.h>
#include "utilities.h"
#include "platform.h"
#include "config.h"

void RunFailed(uint8_t *file, uint32_t line)
{ 
    uint16_t i = 0;

    printf("Run failed: file %s line %d \r\n", file, line);

    for (i = LED0; i < LED_MAX; i++)
    {
#ifdef PY32_PLATFORM
		LED_On((Led_TypeDef)i);
#else
        LED_On((led_typedef_enum)i);
#endif
    }
    
    //Config_EarseFlashData();
    while (1);
}


