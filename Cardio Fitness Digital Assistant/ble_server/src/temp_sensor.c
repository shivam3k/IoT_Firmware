
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

#include "temp_sensor.h"

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


uint16_t Read_Temperature(void){

	uint16_t temp = 0;
	uint16_t temp_LSB = 0;
	uint16_t temp_MSB = 0;
	int32_t temp_Celsius = 0;
	uint8_t slave_address = SI7021_ADDRESS;
	uint8_t d = 0;

	/* Checking for bus busy */
	if(I2C0->STATE & I2C_STATE_BUSY){
		I2C0->CMD = I2C_CMD_ABORT;}

	/* Writing Slave address and write bit to transmit buffer */
	I2C0->TXDATA = ((slave_address << 1)|(I2C0_WRITE_BIT));
	/* Start Command */
	I2C0->CMD = I2C_CMD_START;
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/* Writing Read Command to transmit buffer */
	I2C0->TXDATA = READ_TEMP_CMD;
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/* Repeated Start Command */
	I2C0->CMD = I2C_CMD_START;
	/* Writing Slave address and read bit to transmit buffer */
	I2C0->TXDATA = ((slave_address << 1)|(I2C0_READ_BIT));
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/*Waiting for MSB of the 14-bit temperature reading*/
	while( ((I2C0->STATUS) & I2C_STATUS_RXDATAV)==0 );
	/* Writing MSB of temp reading to temp_MSB variable */
	temp_MSB = I2C0->RXDATA;
	/* Sending ACK command to slave*/
	I2C0->CMD = I2C_CMD_ACK;
	/* Writing LSB of temp reading to temp_LSB variable */
	temp_LSB = I2C0->RXDATA;

	/* Sending NACK command to slave*/
	I2C0->CMD = I2C_CMD_NACK;
	/* Sending STOP command to slave*/
	I2C0->CMD = I2C_CMD_STOP;

	/* Calculating 14-bit temperature value*/
	temp = ((temp_MSB<<8)|(temp_LSB & 0xFC));
	/* Determining temperature in celsius */
	temp_Celsius = (int16_t)((((17572*(temp))>>16)-4685)/100);
	//temp >>= 2;

	/*Setting or clearing LED1 depending on default temp macro*/
	if(temp_Celsius > CHECK_TEMP_CELSIUS){
		GPIO_PinOutClear(LED1_port, LED1_pin);
		GPIO_PinOutSet(LED0_port, LED0_pin);
		//GPIO_PinOutClear(LED0_port, LED0_pin);
		d = 7;
	//unsigned char temp0 = (unsigned char)(temp_Celsius);

	//temp1 = load_ps_flash();
	//if((temp0 >= (temp1 + 2)) && (temp0 < 35)){
		//save_ps_flash(tempflash,temp_Celsius);
		//GPIO_PinOutSet(LED0_port,LED0_pin);
	}


#ifdef SAVE_PS_FLASH

		tempflash.temp_celsius = temp_Celsius;

		struct gecko_msg_flash_ps_save_rsp_t *save_flash_temp;
		save_flash_temp = gecko_cmd_flash_ps_save(0x406F,sizeof(tempflash),(const uint8*)&tempflash);

		if(save_flash_temp->result == 0){
			GPIO_PinOutSet(LED1_port,LED1_pin);
		}

#endif



#ifdef LOAD_PS_FLASH

		struct gecko_msg_flash_ps_load_rsp_t *load_flash_temp;
		load_flash_temp = gecko_cmd_flash_ps_load(0x406F);

		if(load_flash_temp->result == 0){
			unsigned char temp_load = load_flash_temp->value.data[0];
			unsigned char temppp = (unsigned char)(temp_Celsius);
			if(temp_load == temppp){
				GPIO_PinOutSet(LED1_port,LED1_pin);
			}
		}

#endif


	else{
		d = 8;
		GPIO_PinOutSet(LED1_port, LED1_pin);
		GPIO_PinOutClear(LED0_port, LED0_pin);
		//GPIO_PinOutSet(LED1_port, LED1_pin);
	}
return temp;
}

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

void Temp_Sensor_Disable(void){

	/* Disabling SCL and SDA lines */
	GPIO_PinModeSet(I2C0_PORT, I2C0_SCL_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(I2C0_PORT, I2C0_SDA_PIN, gpioModeDisabled, 0);

	/* Clearing Sensor Enable Pin */
	GPIO_PinOutClear(TEMP_PORT, TEMP_PIN);
}


