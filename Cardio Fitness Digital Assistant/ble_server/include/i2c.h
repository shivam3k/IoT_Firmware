
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


#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "em_i2c.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include <stdbool.h>
#include "temp_sensor.h"
#include "sleep.h"
#include "cmu.h"

#define SI7021_SCL_ROUTE				(14)
#define SI7021_SDA_ROUTE				(16)

#define I2C0_PORT						(gpioPortC)
#define I2C0_SCL_PIN					(10)
#define I2C0_SDA_PIN					(11)

#define I2C0_READ_BIT					(1)
#define I2C0_WRITE_BIT					(0)

/*******************************************************************************
 * @brief
 *   I2C0 Routing and Initialization Function
 *
 * @details
 *   This function routes the I2C0 SCL and SDA pins
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void I2C0_Init0(void);

/*******************************************************************************
 * @brief
 *   I2C0 enable function
 *
 * @details
 *   This function enables the I2C0 peripheral
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void I2C0_Init1(void);

/*******************************************************************************
 * @brief
 *   Temperature Sensor enable function
 *
 * @details
 *   This function enables the Si7021 temperature sensor through GPIO
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void Sensor_Power(void);

#endif /* I2C_H_ */
