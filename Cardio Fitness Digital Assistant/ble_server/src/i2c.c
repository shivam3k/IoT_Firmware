
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
// Include files
//***********************************************************************************

#include <stdbool.h>
#include "i2c.h"
#include "temp_sensor.h"

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

void I2C0_Init0(void){

	/* Routing SCL (PC10) and SDA (PC11) pins  */
	I2C0->ROUTEPEN   = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
	I2C0->ROUTELOC0 |= ((I2C_ROUTELOC0_SCLLOC_LOC14)|(I2C_ROUTELOC0_SDALOC_LOC16));

	/* Initializing I2C0 peripheral */
	I2C_Init_TypeDef si7021 = I2C_INIT_DEFAULT;
	I2C_Init(I2C0, &si7021);

	/* Checking for bus busy */
	if(I2C0->STATE & I2C_STATE_BUSY){
			I2C0->CMD = I2C_CMD_ABORT;
		}
}

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

void Sensor_Power(void)
{
	/* Setting GPIO Pin Mode in Push-Pull Configuration*/
	GPIO_PinModeSet(TEMP_PORT, TEMP_PIN, gpioModePushPull, 1);
}

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

void I2C0_Init1(void){

	/* Setting mode as WIRED AND for SCL and SCL pins	  */
	GPIO_PinModeSet(I2C0_PORT,I2C0_SCL_PIN,gpioModeWiredAnd,1);
	GPIO_PinModeSet(I2C0_PORT,I2C0_SDA_PIN,gpioModeWiredAnd,1);

	/* Clearing SCL for sensor */
	for(uint8_t a=0;a<9;a++){
		GPIO_PinOutClear(I2C0_PORT,I2C0_SCL_PIN);
		GPIO_PinOutSet(I2C0_PORT,I2C0_SCL_PIN);
	}
	 /* Clearing Interrupts (if  any) */
	I2C0->IFC=0X07FF;
	/* Enabling ACK and RXDATAV interrupts */
	I2C0->IEN |= (I2C_IEN_ACK|I2C_IEN_RXDATAV);
	/* Enabling I2C0 peripheral */
	I2C_Enable(I2C0, true);

}





