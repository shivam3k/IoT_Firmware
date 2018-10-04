#include "acc_i2c.h"


void Acc_Init(void){

	uint8_t byte;
	//I2C_startTransmission();
	byte = I2C_singleByteRead(WHO_AM_I);

	GRAPHICS_Clear();
	if(byte==0x2A){
		GRAPHICS_AppendString("acc connected");
	}
	else{
		GRAPHICS_AppendString("acc conn failed!");
		while(1);
	}
	GRAPHICS_Update();

	byte = I2C_singleByteRead(CTRL_REG1);
	I2C_singleByteWrite(CTRL_REG1,(byte & (~(0x01))));

	uint8_t g_scale = GSCALE;
	g_scale >>= 2;

	I2C_singleByteWrite(XYZ_DATA_CFG,g_scale);

	byte = I2C_singleByteRead(CTRL_REG1);
	I2C_singleByteWrite(CTRL_REG1,(byte|0x01));

}

void Acc_Transient_X_Init(void){

	uint8_t byte;

	I2C_singleByteWrite(CTRL_REG1, CTRL1_STANDBY);
	I2C_singleByteWrite(TRANSIENT_CFG,XTEFE_TRANSIENT);
	I2C_singleByteWrite(TRANSIENT_THS,THRESHOLD_8_COUNTS);
	I2C_singleByteWrite(TRANSIENT_COUNT,DEBOUNCE_50_MS);
	I2C_singleByteWrite(CTRL_REG4,INT1_ENABLE);
	I2C_singleByteWrite(CTRL_REG5,INT1_ENABLE);

	byte = I2C_singleByteRead(CTRL_REG1);
	byte |= ACTIVE_MODE;

	I2C_singleByteWrite(CTRL_REG1,byte);

}

void Acc_Read(void){

	int acc_count[3];
	read_AccReg(acc_count);
	char buffer[12];
	float acc_g[3];
	GRAPHICS_Clear();
	for(uint8_t i=0;i<3;i++){
		acc_g[i] = ((float)acc_count[i]/((1<<12)/(2*GSCALE)));
		snprintf(buffer, sizeof(buffer), "%f", acc_g[i]);
		GRAPHICS_AppendString(buffer);
	}
	GRAPHICS_Update();

	for(uint8_t i=0;i<20;i++){
		UDELAY_Delay(1000000);
	}

}

void read_AccReg(int *dst){

	uint8_t data[6];
	uint8_t u;
	int g_val;

	for(u=0;u<6;u++){
		data[u] = I2C_singleByteRead(OUT_X_MSB + u);
	}

	for(u=0;u<3;u++){
		g_val = (((data[u*2] << 8) | (data[u*2 + 1])) >> 4);

		if(data[u*2] >0x7F){
			g_val -= 0x1000;
		}
		*(dst + u) = g_val;
	}
}

uint8_t I2C_singleByteRead(uint8_t register_address){

	uint8_t register_data;
	uint8_t slave_address_1 = ACC_ADDRESS;


	/* Writing Slave address and write bit to transmit buffer */
	I2C0->TXDATA = ((slave_address_1 << 1)|(I2C0_WRITE_BIT));
	/* Start Command */
	I2C0->CMD = I2C_CMD_START;
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/* Writing Read Command to transmit buffer */
	I2C0->TXDATA = register_address;
	/* Waiting for Acknowledgement from slave */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/* Repeated Start Command */
	I2C0->CMD = I2C_CMD_START;
	/* Writing Slave address and read bit to transmit buffer */
	I2C0->TXDATA = ((slave_address_1 << 1)|(I2C0_READ_BIT));
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/*Waiting for MSB of the 14-bit temperature reading*/
	while( ((I2C0->STATUS) & I2C_STATUS_RXDATAV)==0 );
	/* Writing MSB of temp reading to temp_MSB variable */
	register_data = I2C0->RXDATA;

	/* Sending NACK command to slave*/
	I2C0->CMD = I2C_CMD_NACK;
	/* Sending STOP command to slave*/
	I2C0->CMD = I2C_CMD_STOP;

	return register_data;

}



void I2C_singleByteWrite(uint8_t register_address, uint8_t data){

	uint8_t slave_address_1 = ACC_ADDRESS;

	/* Writing Slave address and write bit to transmit buffer */
	I2C0->TXDATA = ((slave_address_1 << 1)|(I2C0_WRITE_BIT));
	/* Start Command */
	I2C0->CMD = I2C_CMD_START;
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	I2C0->TXDATA = register_address;
	/* Waiting for Acknowledgement from slave */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	I2C0->TXDATA = data;
	/* Waiting for Acknowledgement from slave */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

	/* Sending STOP command to slave*/
	I2C0->CMD = I2C_CMD_STOP;

}



void I2C_startTransmission(void){

	uint8_t slave_address_1 = ACC_ADDRESS;

	/* Writing Slave address and write bit to transmit buffer */
	I2C0->TXDATA = ((slave_address_1 << 1)|(I2C0_WRITE_BIT));
	/* Start Command */
	I2C0->CMD = I2C_CMD_START;
	/* Waiting for Acknowledgement from temp sensor (slave) */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	/* Clearing IFC ACK bit */
	I2C0->IFC = I2C_IFC_ACK;

}

void I2C0_Acc_Init0(void){

	/* Routing SCL (PC10) and SDA (PC11) pins  */
	I2C0->ROUTEPEN   = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
	I2C0->ROUTELOC0 |= ((I2C_ROUTELOC0_SCLLOC_LOC14)|(I2C_ROUTELOC0_SDALOC_LOC16));

	/* Initializing I2C0 peripheral */
	I2C_Init_TypeDef acc_init = I2C_INIT_DEFAULT;
	I2C_Init(I2C0, &acc_init);

	/* Checking for bus busy */
	if(I2C0->STATE & I2C_STATE_BUSY){
		I2C0->CMD = I2C_CMD_ABORT;
	}
}

void I2C0_Acc_Init1(void){

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

void Acc_Disable(void){

	/* Disabling SCL and SDA lines */
	GPIO_PinModeSet(I2C0_PORT, I2C0_SCL_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(I2C0_PORT, I2C0_SDA_PIN, gpioModeDisabled, 0);

	/* Clearing Sensor Enable Pin */
	GPIO_PinOutClear(TEMP_PORT, TEMP_PIN);
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

void Acc_Power(void){
	/* Setting GPIO Pin Mode in Push-Pull Configuration*/
	GPIO_PinModeSet(TEMP_PORT, TEMP_PIN, gpioModePushPull, 1);
}

