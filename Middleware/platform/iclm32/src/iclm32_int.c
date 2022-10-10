/**
  ******************************************************************************
  * @file    iclm32_int.c
  * @author  iclm team
  * @brief   interrupt services
  ******************************************************************************
  */
#include "iclm32_int.h"
#include "utilities.h"

__ALIGN(64) uintptr_t __mtvt_clic_vector_table[INTERRUPT_ID_MAX];
extern void trap_entry();

int32_t INT_Request(IRQ_T *irqNew) 
{
    if (NULL == irqNew)
    {
        return FAIL;
    }
	if (irqNew->irq >= INTERRUPT_ID_MAX)
    {   
		return FAIL;
    }
	if (irqNew->function == 0)
    {   
		return FAIL;
    }

    __mtvt_clic_vector_table[irqNew->irq] = (uintptr_t)irqNew->function;
    core_int_priority(irqNew->irq, irqNew->priority);
    core_int_enable(irqNew->irq);
    
	return OK;
}

void INT_Init()
{
    uintptr_t mtvec_base = 0;
    uintptr_t mtvt_base = 0;
    
    interrupt_global_disable();
  
    mtvec_base = (uintptr_t)&trap_entry;
    write_csr(mtvec, MTVEC_CLIC_VECTORED_MODE | mtvec_base);
    
    mtvt_base = (uintptr_t)&__mtvt_clic_vector_table;
    write_csr (0x307, (mtvt_base)); /* 0x307 is CLIC CSR number */

    core_int_cfg();
}

void INT_Enable(void) 
{
    interrupt_global_enable();
}


