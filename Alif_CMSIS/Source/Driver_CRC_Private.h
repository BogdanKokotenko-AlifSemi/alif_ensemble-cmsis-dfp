/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef DRIVER_CRC_PRIVATE_H_
#define DRIVER_CRC_PRIVATE_H_

/* System includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project includes */
#include "crc.h"

#define CRC_8_BIT_SIZE           0          /* To select the 8 bit algorithm size  */
#define CRC_16_BIT_SIZE          1          /* To select the 16 bit algorithm size */
#define CRC_32_BIT_SIZE          2          /* To select the 32 bit algorithm size */

/**
 @brief   : CRC Driver states
 */
typedef volatile struct _CRC_DRIVER_STATE {
    uint32_t initialized : 1;                    /* Driver Initialized    */
    uint32_t powered     : 1;                    /* Driver powered        */
    uint32_t reserved    : 30;                   /* Reserved              */
} CRC_DRIVER_STATE;

/**
 * struct CRC_RESOURCES: structure representing a CRC device
 * @regs               : Register address of the CRC
 * @state              : CRC driver state
 */
typedef struct _CRC_RESOURCES
{
    CRC_Type            *regs;      /* CRC register address */
    CRC_DRIVER_STATE     state;     /* CRC Driver state */
}CRC_RESOURCES;

#endif /* DRIVER_CRC_PRIVATE_H_ */
