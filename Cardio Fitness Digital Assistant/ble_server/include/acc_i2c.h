/*
 * acc_i2c.h
 *
 *  Created on: 01-May-2018
 *      Author: defaultuser0
 */

#ifndef SRC_ACC_I2C_H_
#define SRC_ACC_I2C_H_


/***************************************************************************//**
 * @Author Shivam Khandelwal
 * @Professor Keith Graham
 * Source :https://www.silabs.com/documents/public/example-code
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 */

//***********************************************************************************



#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "graphics.h"

#include "i2c.h"
#include "temp_sensor.h"
#include "udelay.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


#define SI7021_SCL_ROUTE				(14)
#define SI7021_SDA_ROUTE				(16)

#define I2C0_PORT						(gpioPortC)
#define I2C0_SCL_PIN					(10)
#define I2C0_SDA_PIN					(11)

#define I2C0_READ_BIT					(1)
#define I2C0_WRITE_BIT					(0)

#define SA0								(1)

#ifdef SA0
	#if (SA0 == 1)
		#define ACC_ADDRESS					(0x1D)
	#else
		#define ACC_ADDRESS					(0x1C)
	#endif
#endif

#define GSCALE 							(8)


/**************************** //// Register Address Macros //// ****************************/

#define OUT_X_MSB 					(0x01)
#define XYZ_DATA_CFG  				(0x0E)
#define WHO_AM_I   					(0x0D)
#define INT_SOURCE					(0x0C)		/* Interrupt source register */
#define TRANSIENT_CFG				(0x1D)		/* Transient configuration register address */
#define TRANSIENT_SRC				(0x1E)		/* Transient Source register address */
#define TRANSIENT_THS				(0x1F)		/* Transient threshold register address */
#define TRANSIENT_COUNT				(0x20)		/* Debounce counter register address */
#define CTRL_REG1  					(0x2A)		/*Control Register 1 address */
#define CTRL_REG4					(0x2D)		/*Control Register 4 address */
#define CTRL_REG5					(0x2E)		/*Control Register 5 address */

/*****************************************************************************************/

/**************************** //// Register Value Macros //// ****************************/

#define ACTIVE_MODE					(0x01)		/* Set the value for active mode */
#define CTRL1_STANDBY				(0x18)		/* CTRL1 set on standby mode */
#define XTEFE_TRANSIENT				(0x12)		/* Latch and X transient enable */
#define YTEFE_TRANSIENT				(0x14)		/* Latch and X transient enable */
#define ZTEFE_TRANSIENT				(0x18)
#define XYTEFE_TRANSIENT			(0x16)
#define YZTEFE_TRANSIENT			(0x1C)
#define XZTEFE_TRANSIENT			(0x1A)
#define XYZTEFE_TRANSIENT			(0x1D)
#define THRESHOLD_8_COUNTS			(0x08)		/* Threshold set to 8 counts */
#define DEBOUNCE_50_MS				(0x05)		/* Debounce counter set for 50 ms */
#define INT1_ENABLE					(0x20) 		/* Enable Interrupt 1 */

/*****************************************************************************************/


void Acc_Init(void);
void Acc_Transient_X_Init(void);
void Acc_Read(void);
void read_AccReg(int *dst);

uint8_t I2C_singleByteRead(uint8_t register_address);
void I2C_singleByteWrite(uint8_t register_address, uint8_t data);

void I2C0_Acc_Init0(void);
void I2C0_Acc_Init1(void);
void Acc_Disable(void);
void Acc_Power(void);







#endif /* SRC_ACC_I2C_H_ */
