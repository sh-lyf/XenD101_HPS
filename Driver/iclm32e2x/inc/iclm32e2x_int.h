/**
  ******************************************************************************
  * @file    iclm32e2x_int.h
  * @author  ting.gao@iclegend.com
  * @brief   iclm32 e2x interrupt header file
  ******************************************************************************
  */
#ifndef __ICLM32E2X_INT_H__
#define __ICLM32E2X_INT_H__

#include "iclm32e2x.h"

#define SYS_INT_LOCAL_0              (SYS_INT_BASE + 0x10UL)
#define SYS_INT_LOCAL_1              (SYS_INT_BASE + 0x14UL)
#define SYS_INT_LOCAL_2              (SYS_INT_BASE + 0x18UL)
#define SYS_INT_LOCAL_3              (SYS_INT_BASE + 0x1CUL)
#define SYS_INT_LOCAL_4              (SYS_INT_BASE + 0x20UL)
#define SYS_INT_LOCAL_5              (SYS_INT_BASE + 0x24UL)
#define SYS_INT_LOCAL_6              (SYS_INT_BASE + 0x28UL)
#define SYS_INT_LOCAL_7              (SYS_INT_BASE + 0x2CUL)
#define SYS_INT_LOCAL_8              (SYS_INT_BASE + 0x30UL)
#define SYS_INT_LOCAL_9              (SYS_INT_BASE + 0x34UL)
#define SYS_INT_LOCAL_10             (SYS_INT_BASE + 0x38UL)
#define SYS_INT_LOCAL_11             (SYS_INT_BASE + 0x3CUL)
#define SYS_INT_LOCAL_12             (SYS_INT_BASE + 0x40UL)
#define SYS_INT_LOCAL_13             (SYS_INT_BASE + 0x44UL)
#define SYS_INT_LOCAL_14             (SYS_INT_BASE + 0x48UL)
#define SYS_INT_LOCAL_15             (SYS_INT_BASE + 0x4CUL)

#define SYS_INT_ENABLE_OFFSET        (16U)

typedef enum SYS_INT_ID
{
    SYS_INT_ID_0 = BIT(0),
    SYS_INT_ID_1 = BIT(1),
    SYS_INT_ID_2 = BIT(2),
    SYS_INT_ID_3 = BIT(3),
    SYS_INT_ID_4 = BIT(4),
    SYS_INT_ID_5 = BIT(5),
    SYS_INT_ID_6 = BIT(6),
    SYS_INT_ID_7 = BIT(7),
    SYS_INT_ID_8 = BIT(8),
    SYS_INT_ID_9 = BIT(9),
    SYS_INT_ID_10 = BIT(10),
    SYS_INT_ID_11 = BIT(11),
    SYS_INT_ID_12 = BIT(12),
    SYS_INT_ID_13 = BIT(13),
    SYS_INT_ID_14 = BIT(14),
    SYS_INT_ID_15 = BIT(15),
} SYS_INT_ID_E;

#define SYS_INT_ID_MAX (16U)

#define INT_ENABLE                     (1U)
#define INT_DISABLE                    (0U)


#define CLINT_CTRL_ADDR             (0x02000000UL)
#define CLINT_MTIMECMP              (0x4000UL)
#define CLINT_MTIME                 (0xBFF8UL)

#define HART0_CLIC_OFFSET                               0x00800000
#define CLIC_INT_PENDING_OFFSET     (0x0UL)
#define CLIC_INT_ENABLE_OFFSET      (0x400UL)
#define CLIC_INT_CFG_OFFSET         (0x800UL)
#define CLIC_CFG_OFFSET             (0xC00UL)

#define HART0_CLIC_BASE_ADDR                            (CLINT_CTRL_ADDR + HART0_CLIC_OFFSET)
#define HART0_CLICINTIP_ADDR(int_num)                   (HART0_CLIC_BASE_ADDR + CLIC_INT_PENDING_OFFSET + int_num)   /* one byte per enable */
#define HART0_CLICINTIE_ADDR(int_num)                   (HART0_CLIC_BASE_ADDR + CLIC_INT_ENABLE_OFFSET + int_num)   /* one byte per enable */
#define HART0_CLICINTCFG_ADDR(int_num)                  (HART0_CLIC_BASE_ADDR + CLIC_INT_CFG_OFFSET + int_num)   /* one byte per enable */
#define HART0_CLICCFG_ADDR                              (HART0_CLIC_BASE_ADDR + CLIC_CFG_OFFSET)   /* one byte per CLIC */
#define CLICCFG_NVBITS(x)                               ((x & 1) << 0)
#define CLICCFG_NLBITS(x)                               ((x & 0xF) << 1)
#define CLICCFG_NMBITS(x)                               ((x & 0x3) << 5)

typedef enum MTVEC_MODE 
{
    MTVEC_DIRECT_MODE = 0,
    MTVEC_VECTORED_MODE = 1,
    MTVEC_CLIC_DIRECT_MODE = 2,
    MTVEC_CLIC_VECTORED_MODE = 3
} MTVEC_MODE_E;

#define LOCAL_INTR(X)        (16 + X)

typedef enum INTERRUPT_ID
{
    INTERRUPT_ID_BASE = 0,
    INTERRUPT_ID_SW   = (INTERRUPT_ID_BASE + 3),
    INTERRUPT_ID_TMR  = (INTERRUPT_ID_BASE + 7),
    INTERRUPT_ID_EXT  = (INTERRUPT_ID_BASE + 11),
    INTERRUPT_ID_CSW  = (INTERRUPT_ID_BASE + 12),
    INTERRUPT_ID_LC0  = (INTERRUPT_ID_BASE + LOCAL_INTR(0)),
    INTERRUPT_ID_LC1  = (INTERRUPT_ID_BASE + LOCAL_INTR(1)),
    INTERRUPT_ID_LC2  = (INTERRUPT_ID_BASE + LOCAL_INTR(2)),
    INTERRUPT_ID_LC3  = (INTERRUPT_ID_BASE + LOCAL_INTR(3)),
    INTERRUPT_ID_LC4  = (INTERRUPT_ID_BASE + LOCAL_INTR(4)),
    INTERRUPT_ID_LC5  = (INTERRUPT_ID_BASE + LOCAL_INTR(5)),
    INTERRUPT_ID_LC6  = (INTERRUPT_ID_BASE + LOCAL_INTR(6)),
    INTERRUPT_ID_LC7  = (INTERRUPT_ID_BASE + LOCAL_INTR(7)),
    INTERRUPT_ID_MAX
} INTERRUPT_ID_E;



#define read_csr(reg) ({ unsigned long __tmp; \
      asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
      __tmp; })
    
#define write_csr(reg, val) ({ \
      if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
        asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
      else \
        asm volatile ("csrw " #reg ", %0" :: "r"(val)); })
    
#define swap_csr(reg, val) ({ unsigned long __tmp; \
      if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
        asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
      else \
        asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
      __tmp; })
    
#define set_csr(reg, bit) ({ unsigned long __tmp; \
      if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
        asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
      else \
        asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
      __tmp; })
    
#define clear_csr(reg, bit) ({ unsigned long __tmp; \
      if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
        asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
      else \
        asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
      __tmp; })


#define MCAUSE_INT         0x80000000
#define MCAUSE_CAUSE       0x000003FF
        
#define MSTATUS_UIE         0x00000001
#define MSTATUS_SIE         0x00000002
#define MSTATUS_HIE         0x00000004
#define MSTATUS_MIE         0x00000008
#define MSTATUS_UPIE        0x00000010
#define MSTATUS_SPIE        0x00000020
#define MSTATUS_HPIE        0x00000040
#define MSTATUS_MPIE        0x00000080
#define MSTATUS_SPP         0x00000100
#define MSTATUS_HPP         0x00000600
#define MSTATUS_MPP         0x00001800
#define MSTATUS_FS          0x00006000
#define MSTATUS_XS          0x00018000
#define MSTATUS_MPRV        0x00020000
#define MSTATUS_PUM         0x00040000
#define MSTATUS_MXR         0x00080000
#define MSTATUS_VM          0x1F000000
#define MSTATUS32_SD        0x80000000

#endif

