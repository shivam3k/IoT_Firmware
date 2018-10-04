/*
 * gpio.h
 *
 *  Created on: 28-Apr-2018
 *      Author: defaultuser0
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "em_gpio.h"
#include "em_core.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin is
#define	LED0_port				gpioPortF
#define LED0_pin				4
#define LED0_default			false 	// off
// LED1 pin is
#define LED1_port				gpioPortF
#define LED1_pin				5
#define LED1_default			false	// off

#define ADC0_port				gpioPortC
#define ADC0_pin				10
#define ADC0_default			false

#define ADC_EN_port				gpioPortD
#define ADC_EN_pin				12
#define ADC_EN_default			true

#define BTN_port				gpioPortF
#define BTN_PB0_pin				6
#define BTN_PB1_pin				7

#define PASSKEY_YES_EVENT		(12)
#define PASSKEY_NO_EVENT		(13)

extern volatile uint8_t button_flag;

//***********************************************************************************
// function prototypes
//***********************************************************************************

void gpio_init(void);


#endif /* SRC_GPIO_H_ */
