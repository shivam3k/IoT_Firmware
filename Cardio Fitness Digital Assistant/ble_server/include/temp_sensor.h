
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

#ifndef SRC_TEMP_SENSOR_H_
#define SRC_TEMP_SENSOR_H_

//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_i2c.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include "native_gecko.h"

#include "sleep.h"
#include "cmu.h"
#include "i2c.h"
#include "gpio.h"

//#define LOAD_PS_FLASH
//#define SAVE_PS_FLASH



//***********************************************************************************
// Macros
//***********************************************************************************

#define CHECK_TEMP_CELSIUS			(15)				// default temperature in celsius

#define SI7021_ADDRESS				(0x40)				// SI7021 Sensor Slave Address
#define READ_TEMP_CMD				(0xE3)				// Read Temperature Command

#define TEMP_PORT					(gpioPortD)			// Sensor enable port
#define TEMP_PIN 					(9)					// Sensor enable pin
#define TEMP_DEFAULT				(0)					// Sensor enable pin default value


/*******************************************************************************
 * @brief
 *   Temperature Read Function
 *
 * @details
 *   This function communicates with the Si7021 temperature sensor and sets the LED1
 *   according to temperature value in celsius.
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

uint16_t Read_Temperature(void);

/*******************************************************************************
 * @brief
 *   Temperature Sensor Disable Function
 *
 * @details
 *   This function disables temperature sensor after reading temperature
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void Temp_Sensor_Disable(void);

//void temp_read(void);

/*******************************************************************************/

#endif /* TEMP_SENSOR_H_ */
