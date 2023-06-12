/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     i2c_using_i3c_Baremetal.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     04-April-2022
 * @brief    Baremetal TestApp to verify i2c communication with
 *           multiple i2c slave devices using i3c IP
 *
 *           hardware setup
 *           BMI Slave is internally connected with the I3C_D
 *           No hardware setup required.
 *           Pins used:
 *           P7_6 (SDA)
 *           P7_7 (SCL)
 *
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include <stdio.h>
#include "Driver_I3C.h"
#include "pinconf.h"

#include <RTE_Components.h>
#include CMSIS_device_header

/* i3c Driver */
extern ARM_DRIVER_I3C Driver_I3C;
static ARM_DRIVER_I3C *I3Cdrv = &Driver_I3C;

/* Comment this if output is to be observed on debugger console via semihosting */
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
                    while (1)
                              ;
          }
#endif

void i2c_using_i3c_demo_thread_entry();

/* i3c callback events */
typedef enum _I3C_CB_EVENT{
    I3C_CB_EVENT_SUCCESS        = (1 << 0),
    I3C_CB_EVENT_ERROR          = (1 << 1),
    I3C_CB_EVENT_MST_TX_DONE    = (1 << 2),
    I3C_CB_EVENT_MST_RX_DONE    = (1 << 3),
    I3C_CB_EVENT_SLV_TX_DONE    = (1 << 4),
    I3C_CB_EVENT_SLV_RX_DONE    = (1 << 5),
    I3C_CB_EVENT_DYN_ADDR_ASSGN = (1 << 6)
}I3C_CB_EVENT;

volatile int32_t cb_event_flag = 0;

/**
  \fn          INT hardware_init(void)
  \brief       i3c hardware pin initialization:
                - PIN-MUX configuration
                - PIN-PAD configuration
  \param[in]   void
  \return      ARM_DRIVER_OK : success; ARM_DRIVER_ERROR : failure
*/
int32_t hardware_init(void)
{

  /* I3C_SDA_D */
  pinconf_set(PORT_7, PIN_6, PINMUX_ALTERNATE_FUNCTION_6,
          PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_PULL_UP | \
          PADCTRL_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS);

  /* I3C_SCL_D */
  pinconf_set( PORT_7, PIN_7, PINMUX_ALTERNATE_FUNCTION_6,
          PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_PULL_UP | \
          PADCTRL_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS);

    return ARM_DRIVER_OK;
}

/**
  \fn          void I3C_callback(UINT event)
  \brief       i3c isr callback
  \param[in]   event: i3c Event
  \return      none
*/
void I3C_callback(uint32_t event)
{
	if (event & ARM_I3C_EVENT_TRANSFER_DONE)
	{
		/* Transfer Success */
		cb_event_flag = I3C_CB_EVENT_SUCCESS;
	}

	if (event & ARM_I3C_EVENT_TRANSFER_ERROR)
	{
		/* Transfer Error */
		cb_event_flag = I3C_CB_EVENT_ERROR;
	}

	if (event & ARM_I3C_EVENT_MST_TX_DONE)
	{
		/* Transfer Success */
		cb_event_flag = I3C_CB_EVENT_MST_TX_DONE;
	}

	if (event & ARM_I3C_EVENT_MST_RX_DONE)
	{
		/* Transfer Success */
		cb_event_flag = I3C_CB_EVENT_MST_RX_DONE;
	}

	if (event & ARM_I3C_EVENT_SLV_TX_DONE)
	{
		/* Transfer Success */
		cb_event_flag = I3C_CB_EVENT_SLV_TX_DONE;
	}

	if (event & ARM_I3C_EVENT_SLV_RX_DONE)
	{
		/* Transfer Success */
		cb_event_flag = I3C_CB_EVENT_SLV_RX_DONE;
	}

	if (event & ARM_I3C_EVENT_SLV_DYN_ADDR_ASSGN)
	{
		/* Transfer Success */
		cb_event_flag = I3C_CB_EVENT_DYN_ADDR_ASSGN;
	}
}

/**
  \fn          void i2c_using_i3c_demo_thread_entry(ULONG thread_input)
  \brief       TestApp to verify i2c communication with
                multiple i2c slave devices using i3c IP.


               This demo thread does:
                 - initialize i3c driver;
                 - set i2c speed mode to Standard mode 100 KBPS;
                 - attach all i2c slave devices to i3c;
                 - continuously read from register address 0x0000(chip-id)
                    for all the attached slaves;
                 - display result depending on whether
                   slave has given ACK or NACK.
  \return      none
*/
void i2c_using_i3c_demo_thread_entry()
{

/* Maximum 8 Slave Devices are supported */
#define MAX_SLAVE_SUPPORTED   3

/* BMI slave address (chip-id 0x43) */
#define BMI_ADDR              0x69

/* Dummy Slave */
#define DUMMY_SLAVE1          0x50
#define DUMMY_SLAVE2          0x55

	int32_t   i      = 0;
	int32_t   ret    = 0;
	int32_t   len    = 0;
	uint32_t  retry_cnt = 0;
	uint8_t   tx_data[2] = {0};   /* transmit data to   i3c */
	uint8_t   rx_data[3] = {0};   /* receive  data from i3c */
	ARM_DRIVER_VERSION version;

    /* array of i2c slave address(static) */
	uint8_t slave_addr[MAX_SLAVE_SUPPORTED] =
	{
	         BMI_ADDR,
		 DUMMY_SLAVE1,
		 DUMMY_SLAVE2
	};

	printf("\r\n \t\t >>> i2c using i3c demo starting up!!! <<< \r\n");

	/* Get i3c driver version. */
	version = I3Cdrv->GetVersion();
	printf("\r\n i3c version api:0x%X driver:0x%X \r\n",  \
                           version.api, version.drv);

	/* Initialize i3c hardware pins using PinMux Driver. */
	ret = hardware_init();
	if(ret != ARM_DRIVER_OK)
	{
		printf("\r\n Error: i3c hardware_init failed.\r\n");
		return;
	}

	/* Initialize I3C driver */
	ret = I3Cdrv->Initialize(I3C_callback);
	if(ret != ARM_DRIVER_OK)
	{
		printf("\r\n Error: I3C Initialize failed.\r\n");
		return;
	}

	/* Power up I3C peripheral */
	ret = I3Cdrv->PowerControl(ARM_POWER_FULL);
	if(ret != ARM_DRIVER_OK)
	{
		printf("\r\n Error: I3C Power Up failed.\r\n");
		goto error_uninitialize;
	}

	/* i2c Speed Mode Configuration:
	 *  I3C_BUS_MODE_MIXED_FAST_I2C_FMP_SPEED_1_MBPS  : Fast Mode Plus   1 MBPS
	 *  I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS : Fast Mode      400 KBPS
	 *  I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS : Standard Mode  100 KBPS
	 */
	ret = I3Cdrv->Control(I3C_MASTER_SET_BUS_MODE,
                           I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS);
	if(ret != ARM_DRIVER_OK)
	{
		printf("\r\n Error: I3C Control failed.\r\n");
		goto error_poweroff;
	}

	/* Attach all the slave address */
	printf("\r\n Start attaching all i2c slave addr to i3c.\r\n");
	for(i=0; i<MAX_SLAVE_SUPPORTED; i++)
	{
		printf("\r\n  >> i=%d attaching i2c slave addr:0x%X to i3c...\r\n",  \
                           i, slave_addr[i]);

		ret = I3Cdrv->AttachI2Cdev(slave_addr[i]);
		if(ret != ARM_DRIVER_OK)
		{
			printf("\r\n Error: I3C Attach I2C device failed.\r\n");
			goto error_poweroff;
		}
	}

	/*
	 * @Note:
	 *  How much data(register address + actual data) user has to Transmit/Receive ?
	 *   it depends on Slave's register address location bytes.
	 *
	 *  First LSB[7-0] will be added to TX FIFO and first transmitted on the i3c bus;
	 *   remaining bytes will be added in LSB -> MSB order.
	 *
	 *  For Slave who supports 16-bit(2 Byte) register address and data:
	 *   Register Address[15:8] : Needs to be Transmit First  to the i3c
	 *   Register Address[07:0] : Needs to be Transmit Second to the i3c
	 *
	 *  That means,
	 *
	 *  While transmitting to TX FIFO,
	 *   MSB of TX data needs to be added first to the TX FIFO.
	 *
	 *  While receiving from RX FIFO,
	 *   First MSB will be received from RX FIFO.
	 *
	 *  START          I3C FIFO           END
	 *  MSB                               LSB
	 *  24-31 bit | 16-23 bit | 8-15 bit | 0-7 bit
	 *
	 *  So, USER has to modify
	 *  Transmit/Receive data (Little Endian <-> Big Endian and vice versa)
	 *  before sending/after receiving to/from i3c TX/RX FIFO.
	 */


	/* Let's Continuously read from register address 0x0000(chip-id) for
	 * all the attached slaves and display received data depending on
	 * whether slave has given ACK or NACK.
	*/
	while(1)
	{
		for(i=0; i<MAX_SLAVE_SUPPORTED; i++)
		{
			/* To Read from register address 0x0000(chip-id):
			 *  First write 0x0000 using MasterTransmit and
			 *   then Read  data   using MasterReceive
			 */
			tx_data[0] = 0x00; /* Register Address[15:8] goes first  */
			tx_data[1] = 0x00; /* Register Address[07:0] goes second */

			/* TX/RX length is 2 Byte
			 * (assume slave requires 16-bit data for TX/RX).
			 */
			len = 2;

			printf("\r\n ------------------------------------------------------------ \r\n");
			printf("\r\n >> i=%d TX slave addr:0x%X reg_addr:[0]0x%X [1]0x%X \r\n",  \
                               i, slave_addr[i], tx_data[0],tx_data[1]);

			/* For TX, User has to pass
			 * Slave Address + TX data + length of the TX data.
			 */
			cb_event_flag = 0;
			ret = I3Cdrv->MasterTransmit(slave_addr[i], tx_data, len);
			if(ret != ARM_DRIVER_OK)
			{
				printf("\r\n Error: i2c Master Transmit failed. \r\n");
				goto error_detach;
			}

			/* wait till any event success/error comes in isr callback */
			retry_cnt = 1000;
			while (retry_cnt--)
			{
				/* delay for 1 milli sec */
				PMU_delay_loop_us (1000);
				if(cb_event_flag == I3C_CB_EVENT_MST_TX_DONE)
				{
					/* TX Success: Got ACK from slave */
					printf("\r\n \t\t >> i=%d TX Success: Got ACK from slave addr:0x%X.\r\n",  \
								   i, slave_addr[i]);
					break;
				}

				if(cb_event_flag == I3C_CB_EVENT_ERROR)
				{
					/* TX Error: Got NACK from slave */
					printf("\r\n \t\t >> i=%d TX Error: Got NACK from slave addr:0x%X \r\n",  \
								   i, slave_addr[i]);
					break;
				}
			}

			if ( (!(retry_cnt)) && (!(cb_event_flag)) )
			{
				printf("Error: event retry_cnt \r\n");
				goto error_detach;
			}

			printf("\r\n\r\n >> i=%d RX slave addr:0x%X \r\n",i, slave_addr[i]);

			/* clear rx data buffer. */
			rx_data[0] = 0;
			rx_data[1] = 0;
			rx_data[2] = 0;

			len = 3;
			/* For RX, User has to pass
			 * Slave Address + Pointer to RX data + length of the RX data.
			 */
			cb_event_flag = 0;
			ret = I3Cdrv->MasterReceive(slave_addr[i], rx_data, len);
			if(ret != ARM_DRIVER_OK)
			{
				printf("\r\n Error: i2c Master Receive failed. \r\n");
				goto error_detach;;
			}

			/* wait till any event success/error comes in isr callback */
			retry_cnt = 1000;
			while (retry_cnt--)
			{
				/* delay for 1 milli sec */
				PMU_delay_loop_us (1000);
				if(cb_event_flag == I3C_CB_EVENT_MST_RX_DONE)
				{
					/* RX Success: Got ACK from slave */
					printf("\r\n \t\t >> i=%d RX Success: Got ACK from slave addr:0x%X.\r\n",  \
								   i, slave_addr[i]);
					printf("\r\n \t\t >> i=%d RX Received Data from slave:[0]0x%X [1]0x%X [2]0x%X\r\n",  \
								   i,rx_data[0],rx_data[1],rx_data[2]);
					break;
				}

				if(cb_event_flag == I3C_CB_EVENT_ERROR)
				{
					/* RX Error: Got NACK from slave */
					printf("\r\n \t\t >> i=%d RX Error: Got NACK from slave addr:0x%X \r\n",  \
								   i, slave_addr[i]);
					break;
				}

			}

			if ( (!(retry_cnt)) && (!(cb_event_flag)) )
			{
				printf("Error: event retry_cnt \r\n");
				goto error_detach;
			}

			printf("\r\n ---------------------------XXX------------------------------ \r\n");

			/* delay for 1 milli sec */
			PMU_delay_loop_us (1000);
		}
	}

error_detach:

	/* Detach all attached slave address */
	for(i=0; i<MAX_SLAVE_SUPPORTED; i++)
	{
		printf("\r\n i=%d detaching i2c slave addr:0x%X from i3c.\r\n",i, slave_addr[i]);
		ret = I3Cdrv->Detachdev(slave_addr[i]);
		if(ret != ARM_DRIVER_OK)
		{
			printf("\r\n Error: I3C Detach I2C device failed.\r\n");
		}
	}

error_poweroff:

	/* Power off I3C peripheral */
	ret = I3Cdrv->PowerControl(ARM_POWER_OFF);
	if(ret != ARM_DRIVER_OK)
	{
		printf("\r\n Error: I3C Power OFF failed.\r\n");
	}

error_uninitialize:

	/* Un-initialize I3C driver */
	ret = I3Cdrv->Uninitialize();
	if(ret != ARM_DRIVER_OK)
	{
		printf("\r\n Error: I3C Uninitialize failed.\r\n");
	}

	printf("\r\nI3C demo thread exiting...\r\n");
}

int main()
{
	i2c_using_i3c_demo_thread_entry ();
	return 0;
}

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
