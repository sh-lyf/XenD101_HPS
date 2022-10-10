/**
  ******************************************************************************
  * @file    iclm32e2x_int.c
  * @author  ting.gao@iclegend.com
  * @brief   iclm32 e2x interrupt driver
  ******************************************************************************
  */
  
#include "iclm32e2x_int.h"

static uint32_t sysIntEnable = 0x00000000;

void system_int_set(uint32_t localx, uint32_t status, uint32_t sysIrq)
{
    uint16_t i = 0;

    for(i = 0U; i < SYS_INT_ID_MAX; i++)
    {
        if((1U << i) & sysIrq)
        {
            sysIntEnable &= ~BIT(i + SYS_INT_ENABLE_OFFSET);
            if (status)
            {
                sysIntEnable |= BIT(i + SYS_INT_ENABLE_OFFSET);
            }
        }
    }

    REG32(localx) = sysIntEnable;
}

void system_int_clear_status(uint32_t localx, uint32_t sysIrq)
{
    uint32_t aaaa = sysIntEnable | sysIrq;
    REG32(localx) = sysIntEnable | sysIrq;
}

uint32_t get_system_int_status(uint32_t localx)
{
    return REG32(localx);
}

void interrupt_global_enable (void) 
{
    uintptr_t m;
    __asm__ volatile ("csrrs %0, mstatus, %1" : "=r"(m) : "r"(MSTATUS_MIE));
}

void interrupt_global_disable (void) 
{
    uintptr_t m;
    __asm__ volatile ("csrrc %0, mstatus, %1" : "=r"(m) : "r"(MSTATUS_MIE));
}

uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc)
{
    if (mcause & MCAUSE_INT)
    {
        //handle interrupt, when MTVEC_DIRECT_MODE
    }
    else
    {
        // handle execption
        while (1);
    }
        
    return epc;
}

void core_int_enable(uint32_t coreIrq)
{
    REG8(HART0_CLICINTIE_ADDR(coreIrq)) = 1;
}

void core_int_priority(uint32_t coreIrq, uint8_t prority)
{
    REG8(HART0_CLICINTCFG_ADDR(coreIrq)) = prority;
}

void core_int_cfg()
{
    uint8_t cliccfg = 0;
    
    cliccfg = (CLICCFG_NVBITS(0) | CLICCFG_NLBITS(0) | CLICCFG_NMBITS(0));
    REG8(HART0_CLICCFG_ADDR) = cliccfg;
}

void schedule_timer_interrupt(unsigned ticks)
{
    volatile uint64_t *mtime = (uint64_t *)(CLINT_CTRL_ADDR + CLINT_MTIME);
    volatile uint64_t *mtimecmp = (uint64_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP);
    *mtimecmp = *mtime + ticks;
}

void _init()
{
    if (read_csr(misa) & (1 << ('F' - 'A'))) // if F extension is present
    { 
        write_csr(mstatus, MSTATUS_FS); // allow FPU instructions without trapping
        write_csr(fcsr, 0); // initialize rounding mode, undefined at reset
    }
}

