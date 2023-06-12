/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     i2c_baremetal.c
 * @brief    Baremetal demo application to verify I2C Master and
 *           Slave functionality without any operating system
 *
 *           Code will verify below cases:
 *            1.)Master transmit 10 bytes and Slave receive 10 bytes
 *            2.)Slave transmit 15 bytes and Master receive 15 bytes
 *           I2C1 instance is taken as Master (PIN used P7_2 and P7_3)
 *           I2C0 instance is taken as Slave  (PIN used P0_2 and P0_3)
 *
 *           Hardware setup:
 *           - Connecting GPIO pins of I2C1 TO I2C0
 *             SDA pin P7_2(J15) to P0_2(J11)
 *             SCL pin P7_3(J15) to P0_3(J11).
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_I2C.h"
#include "pinconf.h"

/* For Release build disable printf and semihosting */
#define DISABLE_PRINTF

#ifdef DISABLE_PRINTF
    #define printf(fmt, ...) (0)
    /* Also Disable Semihosting */
    #if __ARMCC_VERSION >= 6000000
            __asm(".global __use_no_semihosting");
    #elif __ARMCC_VERSION >= 5000000
            #pragma import(__use_no_semihosting)
    #else
            #error Unsupported compiler
    #endif

    void _sys_exit(int return_code) {
            while (1);
    }
#endif

/* I2C Driver instance */
extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C *I2C_MstDrv = &Driver_I2C1;

extern ARM_DRIVER_I2C Driver_I2C0;
static ARM_DRIVER_I2C *I2C_SlvDrv = &Driver_I2C0;

volatile uint32_t mst_cb_status = 0;
volatile uint32_t slv_cb_status = 0;

#define TAR_ADDRS         (0X40)   /* Target(Slave) Address, use by Master */
#define SAR_ADDRS         (0X40)   /* Slave Own Address,     use by Slave  */
#define RESTART           (0X01)
#define STOP              (0X00)

/* master transmit and slave receive */
#define MST_BYTE_TO_TRANSMIT            10

/* slave transmit and master receive */
#define SLV_BYTE_TO_TRANSMIT            15

/* Master parameter set */

/* Master TX Data (Any random value). */
uint8_t MST_TX_BUF[MST_BYTE_TO_TRANSMIT] =
{
    0XAF,0xCE,0xAB,0xDE,0x4A,
    0X22,0X55,0X89,0X46,0X88
};

/* master receive buffer */
uint8_t MST_RX_BUF[SLV_BYTE_TO_TRANSMIT];

/* Master parameter set END  */


/* Slave parameter set */

/* slave receive buffer */
uint8_t SLV_RX_BUF[MST_BYTE_TO_TRANSMIT];

/* Slave TX Data (Any random value). */
uint8_t SLV_TX_BUF[SLV_BYTE_TO_TRANSMIT] =
{
    0X84,0xCD,0x6F,0x5E,0x49,
    0X42,0X2B,0X23,0X46,0X78,
    0X67,0XCC,0xDD,0XAB,0XAE
};

/* Slave parameter set END */


static void i2c_mst_conversion_callback(uint32_t event)
{

      if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
        /* Transfer or receive is finished */
        mst_cb_status = 1;
      }

}

static void i2c_slv_conversion_callback(uint32_t event)
{
      if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
        /* Transfer or receive is finished */
           slv_cb_status = 1;
      }
}

/* Pinmux for B0 */
void hardware_init()
{
    /* I2C0_SDA_A */
    pinconf_set(PORT_0, PIN_2, PINMUX_ALTERNATE_FUNCTION_3,
         (PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_PULL_UP));

    /* I2C0_SCL_A */
    pinconf_set(PORT_0, PIN_3, PINMUX_ALTERNATE_FUNCTION_3,
         (PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_PULL_UP));

    /* I2C1_SDA_C */
    pinconf_set(PORT_7, PIN_2, PINMUX_ALTERNATE_FUNCTION_5,
         (PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_PULL_UP));

    /* I2C1_SCL_C */
    pinconf_set(PORT_7, PIN_3, PINMUX_ALTERNATE_FUNCTION_5,
         (PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_PULL_UP));
}

void I2C_demo()
{
    unsigned long events   = 0;
    int   ret      = 0;
    ARM_DRIVER_VERSION version;
    ARM_I2C_CAPABILITIES capabilities;

    printf("\r\n >>> I2C demo starting up!!! <<< \r\n");

    /* Pinmux */
    hardware_init();

    version = I2C_MstDrv->GetVersion();
    printf("\r\n I2C version api:0x%X driver:0x%X...\r\n",version.api, version.drv);

    /* Initialize Master I2C driver */
    ret = I2C_MstDrv->Initialize(i2c_mst_conversion_callback);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C master init failed\n");
        return;
    }

    /* Initialize Slave I2C driver */
    ret = I2C_SlvDrv->Initialize(i2c_slv_conversion_callback);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C slave init failed\n");
        return;
    }

    /* I2C Master Power control  */
    ret = I2C_MstDrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C Master Power up failed\n");
        goto error_uninitialize;
    }

    /* I2C Slave Power control */
    ret = I2C_SlvDrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C Slave Power up failed\n");
        goto error_uninitialize;
    }

    /* I2C Master Control */
    ret = I2C_MstDrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C master control failed\n");
        goto error_uninitialize;
    }

    /* I2C Slave Control */
    ret = I2C_SlvDrv->Control(ARM_I2C_OWN_ADDRESS, SAR_ADDRS);
     if (ret != ARM_DRIVER_OK)
     {
         printf("\r\n Error: I2C slave control failed\n");
         goto error_uninitialize;
     }

     printf("\n----------------Master transmit/slave receive-----------------------\n");

     /* I2C Slave Receive */
     ret = I2C_SlvDrv->SlaveReceive(SLV_RX_BUF, MST_BYTE_TO_TRANSMIT);
     if (ret != ARM_DRIVER_OK)
     {
         printf("\r\n Error: I2C Slave Receive failed\n");
         goto error_uninitialize;
     }

     /* delay */
     PMU_delay_loop_us(500);

     /* I2C Master Transmit */
     I2C_MstDrv->MasterTransmit(TAR_ADDRS, MST_TX_BUF, MST_BYTE_TO_TRANSMIT, STOP);
     if (ret != ARM_DRIVER_OK)
     {
         printf("\r\n Error: I2C Master Transmit failed\n");
         goto error_uninitialize;
     }

     /* wait for master/slave callback. */
     while(mst_cb_status == 0);
     mst_cb_status = 0;

     while(slv_cb_status == 0);
     slv_cb_status = 0;

     /* Compare received data. */
     if (memcmp(&SLV_RX_BUF, &MST_TX_BUF, MST_BYTE_TO_TRANSMIT))
     {
         printf("\n Error: Master transmit/slave receive failed \n");
         printf("\n ---Stop--- \r\n wait forever >>> \n");
         while(1);
     }

     printf("\n----------------Master receive/slave transmit-----------------------\n");

     /* I2C Master Receive */
     ret = I2C_MstDrv->MasterReceive(TAR_ADDRS, MST_RX_BUF, SLV_BYTE_TO_TRANSMIT, STOP);
     if (ret != ARM_DRIVER_OK)
     {
         printf("\r\n Error: I2C Master Receive failed\n");
         goto error_uninitialize;
     }

     /* I2C Slave Transmit */
     I2C_SlvDrv->SlaveTransmit(SLV_TX_BUF, SLV_BYTE_TO_TRANSMIT);
     if (ret != ARM_DRIVER_OK)
     {
         printf("\r\n Error: I2C Slave Transmit failed\n");
         goto error_uninitialize;
     }

     /* wait for master/slave callback. */
     while(mst_cb_status == 0);
     mst_cb_status = 0;

     while(slv_cb_status == 0);
     slv_cb_status = 0;

     /* Compare received data. */
     if(memcmp(&SLV_TX_BUF, &MST_RX_BUF, SLV_BYTE_TO_TRANSMIT))
     {
         printf("\n Error: Master receive/slave transmit failed\n");
         printf("\n ---Stop--- \r\n wait forever >>> \n");
         while(1);
     }

     ret =I2C_MstDrv->Uninitialize();
     if (ret == ARM_DRIVER_OK)
     {
         printf("\r\n I2C Master Uninitialized\n");
         goto error_uninitialize;
     }
     ret =I2C_SlvDrv->Uninitialize();
     if (ret == ARM_DRIVER_OK)
     {
         printf("\r\n I2C Slave Uninitialized\n");
         goto error_uninitialize;
     }

     printf("\n >>> I2C conversion completed without any error <<< \n");
     printf("\n ---END--- \r\n wait forever >>> \n");
     while(1);

  error_poweroff:
      /* Power off I2C peripheral */
      ret = I2C_MstDrv->PowerControl(ARM_POWER_OFF);
      if(ret != ARM_DRIVER_OK)
      {
         printf("\r\n Error: I2C Master Power OFF failed.\r\n");
      }
      ret = I2C_SlvDrv->PowerControl(ARM_POWER_OFF);
      if(ret != ARM_DRIVER_OK)
      {
         printf("\r\n Error: I2C Slave Power OFF failed.\r\n");
      }

  error_uninitialize:
      /* Un-initialize I2C driver */
      ret = I2C_MstDrv->Uninitialize();
      if(ret == ARM_DRIVER_OK)
      {
        printf("\r\n I2C Master Uninitialize\r\n");
      }
      ret = I2C_SlvDrv->Uninitialize();
      if(ret == ARM_DRIVER_OK)
      {
        printf("\r\n I2C Slave Uninitialize\r\n");
      }
      printf("\r\n I2C demo thread exiting...\r\n");
}

int main (void)
{
    I2C_demo();
    while(1);
}
