/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     sys_ctrl_dma.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     18-Apr-2023
 * @brief    System Control Device information for DMA.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef SYS_CTRL_DMA_H

#define SYS_CTRL_DMA_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "RTE_Components.h"
#include CMSIS_device_header

#ifdef  __cplusplus
extern "C"
{
#endif

static inline void enable_dma0_periph_clk(void)
{
    CLKCTL_PER_MST->PERIPH_CLK_ENA |= PERIPH_CLK_ENA_DMA_CKEN;
}

static inline void disable_dma0_periph_clk(void)
{
    CLKCTL_PER_MST->PERIPH_CLK_ENA &= ~PERIPH_CLK_ENA_DMA_CKEN;
}

static inline void set_dma0_boot_manager_sec(void)
{
    CLKCTL_PER_MST->DMA_CTRL &= ~DMA_CTRL_BOOT_MANAGER;
}

static inline void set_dma0_boot_manager_nsec(void)
{
    CLKCTL_PER_MST->DMA_CTRL |= DMA_CTRL_BOOT_MANAGER;
}

static inline void set_dma0_boot_irq_ns_mask(uint32_t boot_irq_ns_mask)
{
    CLKCTL_PER_MST->DMA_IRQ = boot_irq_ns_mask;
}

static inline void set_dma0_boot_periph_ns_mask(uint32_t boot_periph_ns_mask)
{
    CLKCTL_PER_MST->DMA_PERIPH = boot_periph_ns_mask;
}

static inline void reset_dma0(void)
{
    CLKCTL_PER_MST->DMA_CTRL |= DMA_CTRL_SW_RST;
}

static inline void enable_dmalocal_periph_clk(void)
{
    M55LOCAL_CFG->CLK_ENA |= CLK_ENA_DMA_CKEN;
}

static inline void disable_dmalocal_periph_clk(void)
{
    M55LOCAL_CFG->CLK_ENA &= ~CLK_ENA_DMA_CKEN;
}

static inline void set_dmalocal_boot_manager_sec(void)
{
    M55LOCAL_CFG->DMA_CTRL &= ~DMA_CTRL_BOOT_MANAGER;
}

static inline void set_dmalocal_boot_manager_nsec(void)
{
    M55LOCAL_CFG->DMA_CTRL |= DMA_CTRL_BOOT_MANAGER;
}

static inline void set_dmalocal_boot_irq_ns_mask(uint32_t boot_irq_ns_mask)
{
    M55LOCAL_CFG->DMA_IRQ = boot_irq_ns_mask;
}

static inline void set_dmalocal_boot_periph_ns_mask(uint32_t boot_periph_ns_mask)
{
    M55LOCAL_CFG->DMA_PERIPH = boot_periph_ns_mask;
}

static inline void reset_dmalocal(void)
{
    M55LOCAL_CFG->DMA_CTRL |= DMA_CTRL_SW_RST;
}

#ifdef  __cplusplus
}
#endif
#endif /* SYS_CTRL_DMA_H */
