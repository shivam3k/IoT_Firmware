/*************************************************************************************************
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "em_letimer.h"
#include "em_core.h"
#include "em_device.h"
#include "em_prs.h"
#include "em_ldma.h"
#include "em_usart.h"
#include "em_cryotimer.h"
#include "infrastructure.h"
#include "retargetserial.h"


//***********************************************************************************
// defined files
//***********************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "src/gpio.h"
#include "src/cmu.h"
#include "src/letimer.h"
#include "src/acc_i2c.h"
#include "src/temp_sensor.h"
#include "src/i2c.h"
#include "src/pulse_adc.h"
#include "src/adc_prs.h"
#include "src/adc_ldma.h"


#include "graphics.h"
#include "sleep.h"
#include "udelay.h"

/*************************************************************************************************
 * @addtogroup Application
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];


// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};


//***********************************************************************************
// Macro for energy mode
//***********************************************************************************

#define TX_MAX						(80)
#define TX_MIN						(-260)
#define CONN_INTERVAL				(75)
#define SLAVE_INTERVAL				(450)
#define SLAVE_LATENCY				((SLAVE_INTERVAL/CONN_INTERVAL)-1)
#define CONN_VALUE					(CONN_INTERVAL/1.25)
#define ADVERTISING_INTERVAL		(1200)

/******************************************************************************/
/*********************** Persistent Memory Macro ******************************/

#define PERSISTENT_MEMORY

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*********************** LDMA Enable Macro ******************************/

#define LDMA_ENABLE

/******************************************************************************/
/******************************************************************************/

/* Flag for indicating DFU Reset must be performed */
uint8_t boot_to_dfu = 0;

/* Variable to store connection ID*/
uint8_t BLE_CONNECT = 0xFF;

/* Variables to show data on LCD */
char display_number[4];
uint32 display_uint32;

/* Variables to store pulse reading */
int pulse_reading = 0;
uint8_t pulse_val = 0;

char acc_buff[4];
uint8_t step_buffer[4];
uint8_t *steps = step_buffer;


/************************** LCD Display Function ******************************/

void lcd_show(char * data,bool lcd_clear)
{
	if(lcd_clear){GRAPHICS_Clear();}
    GRAPHICS_AppendString(data);
    GRAPHICS_Update();
}

/******************************************************************************/

/*********************Gecko External Signal Functions *************************/

////////////////////// LETIMER EXTERNAL SIGNAL /////////////////////////////////

/***************************************************************
 * @brief: show step count on LCD display after every 5 seconds
****************************************************************/

void acc_display(void){

	lcd_show("Step Count -",true);
	snprintf(acc_buff, sizeof(acc_buff), "%ld", step_count);
	lcd_show(acc_buff,false);
	//lcd_show("",true);

}

////////////////// ACCELEROMETER EXTERNAL SIGNAL (FROM GPIO INT) //////////////

/***********************************************************************
 * @brief: increments step count variable in case of a step count
 * 			by reading the interrupt source and transient source
 * 			registers
************************************************************************/

void accelerometer_int_handle(void){

	uint8_t intSourceSystem = 0;
	uint8_t intSourceTransient = 0;

	intSourceSystem = I2C_singleByteRead(INT_SOURCE);

	if((intSourceSystem & 0x20) == 0x20){

		intSourceTransient = I2C_singleByteRead(TRANSIENT_SRC);
		step_count++;
	}
}

////////////////// PULSE CAPTURE EXTERNAL SIGNAL (FROM LDMA INT) //////////////

/***********************************************************************
 * @brief: increments step count variable in case of a step count
 * 			by reading the interrupt source and transient source
 * 			registers
************************************************************************/

void pulse_capture(void){

#ifdef LDMA_ENABLE

	Pulse_Processing_dma();
	LDMA_loop();

#endif

}

/********************* Persistent Memory Functions ***************************/

#ifdef PERSISTENT_MEMORY

	typedef struct{
		int16_t data;
	}data_ps;

	data_ps data_save;

	bool flash_save(data_ps save,int16_t s,uint32 key){

		bool check = false;
		save.data = s;
		struct gecko_msg_flash_ps_save_rsp_t *save_data;
		save_data = gecko_cmd_flash_ps_save(key,sizeof(save),(const uint8*)&save);

		if(save_data->result == 0){
			check = true;
		}
		return check;
	}

	unsigned char flash_load(uint32 key){

		unsigned char load = 0;
		struct gecko_msg_flash_ps_load_rsp_t *load_data;
				load_data = gecko_cmd_flash_ps_load(key);

		if(load_data->result == 0){
			load = load_data->value.data[0];
		}
		return load;
	}

#endif

/***************************Main Function ****************************************/
/*********************************************************************************/

int main(void){

	  /* Initialize device*/
	  initMcu();

	  // Initialize board
	  initBoard();

	  /* Initialize GPIO */
	  gpio_init();

	  cmu_init2();

	  // Initialize stack
	  gecko_init(&config);

	  GRAPHICS_Init();

	  lcd_show("IOT PROJECT",true);
	  lcd_show("  DEMO",false);

	  UDELAY_Delay(2000000);

	  //lcd_show("Press PB1",true);
	  //lcd_show("to begin",false);

	  ///while((GPIO_PinInGet(BTN_port,BTN_PB1_pin)) != 0);

	  lcd_show("Cardio workout",true);
	  lcd_show("  Assisstant",false);

	  UDELAY_Delay(1000000);

#ifdef PERSISTERNT_MEMORY

	  lcd_show("Loading",true);
	  	  lcd_show("Persistent",false);
	  	  lcd_show("Data",false);
	  	  lcd_show(" ", false);
	  	  lcd_show("Steps",false);

	  	for(uint16 q = 0;q < 100;q++){
	  		  UDELAY_Delay(1500000);}

	  	char wer[6];
	  	unsigned char ld = 7;
	  	ld = flash_load(0x406A);

	  	step_count = (uint32_t)(ld);

	  	lcd_show("STEP COUNT -",true);
	  	snprintf(wer, sizeof(wer), "%ld", step_count);
	  	lcd_show(wer,false);
	  	for(uint16 q = 0;q < 100;q++){
	  	UDELAY_Delay(1500000);}
#endif


#ifndef LDMA_ENABLE
	  /*Initialize Pulse ADC Sensor */
	  Pulse_Init();
#endif
	  /* Accelerometer initialize */
	  I2C0_Acc_Init0();
	  I2C0_Acc_Init1();
	  Acc_Init();
	  Acc_Transient_X_Init(); // Accelerometer working in transient mode


	  /*LETIMER setup config*/
	  LETIMER0_setup();

	  /*enable LETIMER*/
	  LETIMER0_enable();


#ifdef LDMA_ENABLE

	  /* Initialize CRYOTIMER for PRS */
	  Cryo_ADC_Trigger_Init();

	  /* Initialize ADC */
	  LDMA_ADC_init();

	 pulse_flag = 0;

	  /* Initialize LDMA */
	 LDMA_init();

	  LDMA_loop();



#endif

	  unsigned int che = 0;

	  while((che = GPIO_PinInGet(BTN_port,BTN_PB1_pin)) != 0){
  //while (1){

      /* Event pointer for handling events */
      struct gecko_cmd_packet* evt;

      /* Check for stack event. */
      	evt = gecko_wait_event();

      /* Handle events */
      switch (BGLIB_MSG_ID(evt->header)){
        /* This boot event is generated when the system boots up after reset.
         * Do not call any stack commands before receiving the boot event.
         * Here the system is set to start advertising immediately after boot procedure. */
        case gecko_evt_system_boot_id:

        	lcd_show("Boot",true);

        	/* delete all bondings to force the pairing process */
        	gecko_cmd_sm_delete_bondings();

        	lcd_show("All bonding deleted",false);

        	/* Numeric comparison */
        	gecko_cmd_sm_configure(0x0F, sm_io_capability_displayyesno);

        	 /* enable bondable to accommodate certain mobile OS */
        	gecko_cmd_sm_set_bondable_mode(1);

        	lcd_show("Bond mode - \r\n1",false );


          /* Set advertising parameters. 100ms advertisement interval. All channels used.
           * The first two parameters are minimum and maximum advertising interval, both in
           * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
          gecko_cmd_le_gap_set_adv_parameters(ADVERTISING_INTERVAL, ADVERTISING_INTERVAL, 7);

          /* Start general advertising and enable connections. */
          gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
#ifndef LDMA_ENABLE
          gecko_cmd_hardware_set_soft_timer(((32768*20)/1000),0,0);
#endif
          break;

#ifndef LDMA_ENABLE

        /* This event is generated when a soft timer expires */
        case gecko_evt_hardware_soft_timer_id:

        	pulse_reading = Pulse_Processing();

        	if((pulse_reading > 0) && (pulse_reading < 150)){
        		pulse_val = (uint8_t)(pulse_reading);
        		gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_pulse_rate,1,&pulse_val);

        	}
        	else{
        		pulse_val = 0;
        	}
        	uint8_t stp = (uint8_t)(step_count);
        	gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_step_shivam,1,&stp);
        	break;
#endif

        case gecko_evt_le_connection_closed_id:

          /* Check if need to boot to dfu mode */
          if (boot_to_dfu) {
            /* Enter to DFU OTA mode */
            gecko_cmd_system_reset(2);
          }
          else {
        	BLE_CONNECT = 0xFF;
            /* Restart advertising after client has disconnected */
            gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
            lcd_show("Connection",true);
            lcd_show("closed",false);
          }
          break;

        case gecko_evt_le_connection_opened_id:
        	lcd_show("Connected",true);
        	BLE_CONNECT = evt->data.evt_le_connection_opened.connection;
        	gecko_cmd_sm_increase_security(BLE_CONNECT);
        	gecko_cmd_le_connection_set_parameters(BLE_CONNECT,CONN_VALUE,CONN_VALUE,SLAVE_LATENCY,0x300);
        	break;

        case gecko_evt_sm_confirm_passkey_id:

        	lcd_show("Confirm passkey",true);
        	display_uint32 = evt->data.evt_sm_passkey_display.passkey;
        	snprintf(display_number, sizeof(display_number), "%ld", display_uint32);
        	lcd_show(display_number,false);
        	lcd_show("Y: PB0",false);
        	unsigned int pass_check = 5;

        	while((pass_check = GPIO_PinInGet(BTN_port,BTN_PB0_pin)) != 0);
        	gecko_cmd_sm_passkey_confirm(BLE_CONNECT, 1);
        	GPIO_PinOutSet(LED0_port,LED0_pin);

        	break;

        case gecko_evt_sm_bonded_id:

        	lcd_show("Bonding",true);
        	lcd_show("completed",false);

        	break;

        case gecko_evt_sm_bonding_failed_id:

            /* If the attempt at bonding/pairing failed, clear the bonded flag and display the reason */
            lcd_show("Bonding failed",true);

            break;

        case gecko_evt_system_external_signal_id:

        	 if (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_ACCELEROMETER_INT)
        	 {
        		 accelerometer_int_handle();
        	 }
        	 if (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_PULSE_PROCESSING)
        	 {
        		 acc_display();
        	 }

        	 if (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_PULSE_PROCESSING)
        	 {
        	    pulse_capture();
        	 }
        	 break;


        case gecko_evt_le_connection_rssi_id:

        	gecko_cmd_system_halt(1);
        	if(evt->data.evt_le_connection_rssi.rssi > -35){
        		gecko_cmd_system_set_tx_power(TX_MIN);
        	}

        	else if((evt->data.evt_le_connection_rssi.rssi > -45) && (evt->data.evt_le_connection_rssi.rssi < -35)){
        	        		gecko_cmd_system_set_tx_power(-200);
        	        	}
        	else if((evt->data.evt_le_connection_rssi.rssi > -55) && (evt->data.evt_le_connection_rssi.rssi < -45)){
        	        		gecko_cmd_system_set_tx_power(-150);
        	        	}
        	else if((evt->data.evt_le_connection_rssi.rssi > -65) && (evt->data.evt_le_connection_rssi.rssi < -55)){
        	        		gecko_cmd_system_set_tx_power(-50);
        	        	}
        	else if((evt->data.evt_le_connection_rssi.rssi > -75) && (evt->data.evt_le_connection_rssi.rssi < -65)){
        	        		gecko_cmd_system_set_tx_power(0);
        	        	}
        	else if((evt->data.evt_le_connection_rssi.rssi > -85) && (evt->data.evt_le_connection_rssi.rssi < -75)){
        	        	    gecko_cmd_system_set_tx_power(50);
        	        	}
        	else{
        		gecko_cmd_system_set_tx_power(TX_MAX);
        	}
        	gecko_cmd_system_halt(0);

        	break;



        /* Events related to OTA upgrading
           ----------------------------------------------------------------------------- */

        /* Check if the user-type OTA Control Characteristic was written.
         * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
        case gecko_evt_gatt_server_user_write_request_id:

          if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
            /* Set flag to enter to OTA mode */
            boot_to_dfu = 1;
            /* Send response to Write Request */
            gecko_cmd_gatt_server_send_user_write_response(
              evt->data.evt_gatt_server_user_write_request.connection,
              gattdb_ota_control,
              bg_err_success);

            /* Close connection to enter to DFU OTA mode */
            gecko_cmd_endpoint_close(evt->data.evt_gatt_server_user_write_request.connection);
          }
          break;

        default:
        	break;

      }
   }
#ifdef PERSISTENT_MEMORY
	  int16_t ct = (int16_t)(step_count);
	  flash_save(data_save,ct,0x406A);
#endif

}
