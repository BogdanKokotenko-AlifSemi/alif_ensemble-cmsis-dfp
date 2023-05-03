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
 * @file     Driver_SPI_Private.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     20-04-2023
 * @brief    Header file for SPI.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef DRIVER_SPI_PRIVATE_H
#define DRIVER_SPI_PRIVATE_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_SPI.h"

#include "spi.h"

#define SPI_IRQ_NUM(instance)                         (SPI0_IRQ_IRQn + instance)

/**
 * enum SPI_INSTANCE.
 * SPI instances.
 */
typedef enum _SPI_INSTANCE
{
    SPI_INSTANCE_0,                         /**< SPI instance - 0 */
    SPI_INSTANCE_1,                         /**< SPI instance - 1 */
    SPI_INSTANCE_2,                         /**< SPI instance - 2 */
    SPI_INSTANCE_3                          /**< SPI instance - 3 */
} SPI_INSTANCE;

/** \brief SPI Driver states. */
typedef volatile struct _SPI_DRIVER_STATE {
    uint32_t initialized : 1; /* Driver Initialized*/
    uint32_t powered     : 1; /* Driver powered */
    uint32_t reserved    : 30;/* Reserved */
} SPI_DRIVER_STATE;

/** \brief Resources for a SPI instance. */
typedef struct _SPI_RESOURCES
{
    SPI_Type                    *regs;              /**< Pointer to regs                            */
    ARM_SPI_SignalEvent_t       cb_event;           /**< Pointer to call back function              */
    SPI_INSTANCE                drv_instance;       /**< Driver instance                            */
    SPI_FRF                     spi_frf;            /**< SPI frame format - Standard/Dual/Quad/Octal*/
    ARM_SPI_STATUS              status;             /**< SPI driver status                          */
    SPI_DRIVER_STATE            state;              /**< SPI driver state                           */
    spi_transfer_t              transfer;           /**< Transfer structure for the SPI instance    */
    bool                        sw_slave_select;    /**< operate Slave select pin through SW        */
    uint8_t                     irq_priority;       /**< Interrupt priority                         */
    uint8_t                     slave_select;       /**< chip selection pin from 0-3                */
    uint8_t                     rx_fifo_threshold;  /**< Rx FIFO threshold                          */
    uint16_t                    tx_fifo_start_level;/**< TX FIFO start level                        */
    uint8_t                     tx_fifo_threshold;  /**< Tx FIFO threshold                          */
} SPI_RESOURCES;

/**
  \fn          static inline uint32_t getSpiCoreClock(void)
  \brief       Get the SPI input clock rate
  \return      SPI input clock rate
*/
static inline uint32_t getSpiCoreClock(void)
{
    return AHB_CLOCK;
}

#ifdef  __cplusplus
}
#endif

#endif /* DRIVER_SPI_PRIVATE_H */
