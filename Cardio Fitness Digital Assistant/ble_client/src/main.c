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

/*********************************************************************************************
 * @reference - This code has been referred from example code SPP over BLE available on
 * 				Silicon Labs forum.
 *********************************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/* Device initialization header */
#include "hal-config.h"

#include "gpio.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
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

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

#include "graphics.h"

#define DISCONNECTED    0
#define SCANNING        1
#define FIND_SERVICE    2
#define FIND_CHAR       3
#define ENABLE_NOTIF    4
#define DATA_MODE       5
#define DISCONNECTING   6

#define PULSE_CHECK
//#define ACC_CHECK


char display_number[4];
uint32 display_uint32;


uint8_t BLE_CONNECT = 0xFF;

#ifdef PULSE_CHECK

//Service ID = 8c5129fe-c0af-47b9-8946-17f1048617a0

const uint8 pulse_service_UUID[16] = {0xa0, 0x17, 0x86, 0x04, 0xf1, 0x17, 0x46, 0x89, 0xb9, 0x47, 0xaf, 0xc0, 0xfe, 0x29, 0x51, 0x8c};

//Characteristic ID = a7fc2018-afe5-4147-88f1-c9be9acfb05d

const uint8 pulse_characteristic_UUID[16] = {0x5d, 0xb0, 0xcf, 0x9a, 0xbe, 0xc9, 0xf1, 0x88, 0x47, 0x41, 0xe5, 0xaf, 0x18, 0x20, 0xfc, 0xa7};

#endif

#ifdef ACC_CHECK
//
//Service ID = 50279dc2-ab03-4034-8836-b87394fc1e43

const uint8 pulse_service_UUID[16] = {0x43, 0x1e, 0xfc, 0x94, 0x73, 0xb8, 0x36, 0x88, 0x34, 0x40, 0x03, 0xab, 0xc2, 0x9d, 0x27, 0x50};
//
//Characteristic ID = 34d1922a-21da-4bab-ba8c-6c0a112c4cf3

const uint8 pulse_characteristic_UUID[16] = {0xf3, 0x4c, 0x2c, 0x11, 0x0a,0x6c, 0x8c, 0xba, 0xab, 0x4b, 0xda, 0x21, 0x2a, 0x92, 0xd1,0x34};

#endif

static uint8 _conn_handle = 0xFF;
static int _main_state;
static uint32 _service_handle;
static uint16 _char_handle;

int pulse_val;

static void reset_variables()
{
    _conn_handle = 0xFF;
    _main_state = DISCONNECTED;
    _service_handle = 0;
    _char_handle = 0;
}

/************************** LCD Display Function ******************************/


void lcd_show(char * data,bool lcd_clear)
{
	if(lcd_clear){GRAPHICS_Clear();}
    GRAPHICS_AppendString(data);
    GRAPHICS_Update();
}


/******************************************************************************/


static int process_scan_response(struct gecko_msg_le_gap_scan_response_evt_t *pResp)
{
    // decoding advertising packets is done here. The list of AD types can be found
    // at: https://www.bluetooth.com/specifications/assigned-numbers/Generic-Access-Profile

    int i = 0;
    int ad_match_found = 0;
    int ad_len;
    int ad_type;

    char name[32];

    while (i < (pResp->data.len - 1))
    {

        ad_len  = pResp->data.data[i];
        ad_type = pResp->data.data[i+1];

        if (ad_type == 0x08 || ad_type == 0x09 )
        {
            // type 0x08 = Shortened Local Name
            // type 0x09 = Complete Local Name
            memcpy(name, &(pResp->data.data[i+2]), ad_len-1);
            name[ad_len-1] = 0;
            lcd_show(name,true);

        }

        if (ad_type == 0x06 || ad_type == 0x07)
        {
            // type 0x06 = Incomplete List of 128-bit Service Class UUIDs
            // type 0x07 = Complete List of 128-bit Service Class UUIDs
            if(memcmp(pulse_service_UUID, &(pResp->data.data[i+2]),16) == 0)
            {
                lcd_show("Found shivam_health\r\n",true);
                ad_match_found = 1;
            }
        }

        //jump to next AD record
        i = i + ad_len + 1;
    }

    return(ad_match_found);
}

void main(void)
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Initialize stack
  gecko_init(&config);

  char printbuf[128];
  char pulse_buff[10];

  GRAPHICS_Init();
  //int i;

  while (1) {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;

    /* Check for stack event. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header)) {
      /* This boot event is generated when the system boots up after reset.
       * Do not call any stack commands before receiving the boot event.
       * Here the system is set to start advertising immediately after boot procedure. */
      case gecko_evt_system_boot_id:

    	  lcd_show("Boot",true);

    	  /* delete all bondings to force the pairing process */
    	  gecko_cmd_sm_delete_bondings();

    	  lcd_show("All bonding deleted",false);

    	  /* Numeric comparison */
    	  gecko_cmd_sm_configure(0x07, sm_io_capability_displayyesno);

    	  /* enable bondable mode to accommodate certain mobile OS */
    	  gecko_cmd_sm_set_bondable_mode(1);

    	  lcd_show("Bond mode - \r\n1",false );

          gecko_cmd_gatt_set_max_mtu(247);
          gecko_cmd_le_gap_discover(le_gap_discover_generic);
          _main_state = SCANNING;

          break;

      case gecko_evt_sm_confirm_passkey_id:

    	  lcd_show("Confirm passkey",true);

      	display_uint32 = evt->data.evt_sm_confirm_passkey.passkey;
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


      case gecko_evt_le_gap_scan_response_id:

          if(process_scan_response(&(evt->data.evt_le_gap_scan_response)) > 0)
          {
              struct gecko_msg_le_gap_open_rsp_t *pResp;

              // match found -> stop discovery and try to connect
              gecko_cmd_le_gap_end_procedure();

              pResp = gecko_cmd_le_gap_open(evt->data.evt_le_gap_scan_response.address, evt->data.evt_le_gap_scan_response.address_type);

              // make copy of connection handle for later use (for example, to cancel the connection attempt)
              _conn_handle = pResp->connection;

          }

          break;

      case gecko_evt_le_connection_opened_id:

    	  lcd_show("Connected",true);
    	  BLE_CONNECT = evt->data.evt_le_connection_opened.connection;

          gecko_cmd_gatt_discover_primary_services_by_uuid(_conn_handle, 16, pulse_service_UUID);
          _main_state = FIND_SERVICE;

          break;

      case gecko_evt_le_connection_closed_id:

    	  BLE_CONNECT = 0xFF;

    	  lcd_show("Connection",true);
    	  lcd_show("closed",false);

          reset_variables();
          gecko_cmd_le_gap_discover(le_gap_discover_generic);

          break;

      case gecko_evt_gatt_service_id:

          if(evt->data.evt_gatt_service.uuid.len == 16)
          {
              if(memcmp(pulse_service_UUID, evt->data.evt_gatt_service.uuid.data,16) == 0)
              {
            	  lcd_show("service discovered",true);
                  _service_handle = evt->data.evt_gatt_service.service;
              }
          }

          break;

      case gecko_evt_gatt_procedure_completed_id:

          switch(_main_state)
          {
          case FIND_SERVICE:

              if (_service_handle > 0)
              {
                  // Service found, next search for characteristics
                  gecko_cmd_gatt_discover_characteristics(_conn_handle, _service_handle);
                  _main_state = FIND_CHAR;
              }
              else
              {
                  // no service found -> disconnect
            	  lcd_show("Pulse service not found?",true);
                  gecko_cmd_endpoint_close(_conn_handle);
              }

              break;

          case FIND_CHAR:
              if (_char_handle > 0)
              {
                  // Char found, turn on indications
                  gecko_cmd_gatt_set_characteristic_notification(_conn_handle, _char_handle, gatt_notification);
                  _main_state = ENABLE_NOTIF;
              }
              else
              {
                  // no characteristic found? -> disconnect
            	  lcd_show("Pulse rate ",true);
            	  lcd_show("char not found?",false);
                  gecko_cmd_endpoint_close(_conn_handle);
              }
              break;

          default:
              break;
          }
          break;

          break;

      case gecko_evt_gatt_characteristic_id:

          if(evt->data.evt_gatt_characteristic.uuid.len == 16)
          {
              if(memcmp(pulse_characteristic_UUID, evt->data.evt_gatt_characteristic.uuid.data,16) == 0)
              {
            	  lcd_show("char discovered",true);
                  _char_handle = evt->data.evt_gatt_characteristic.characteristic;
              }
          }

      break;

      case gecko_evt_gatt_characteristic_value_id:

          if(evt->data.evt_gatt_characteristic_value.characteristic == _char_handle)
          {
              memcpy(printbuf, evt->data.evt_gatt_characteristic_value.value.data, evt->data.evt_gatt_characteristic_value.value.len);

              printbuf[evt->data.evt_gatt_characteristic_value.value.len] = 0;

              pulse_val = (int)(printbuf[0]);

             snprintf(pulse_buff, sizeof(pulse_buff), "%d", pulse_val);
             lcd_show("CHAR VALUE -",true);
             lcd_show(pulse_buff,false);
          }
      break;

      default:
        break;
    }
  }
}
