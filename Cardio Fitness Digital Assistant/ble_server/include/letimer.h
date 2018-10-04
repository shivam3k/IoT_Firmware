
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


#ifndef SRC_LETIMER_H
#define SRC_LETIMER_H

/************************* Include Files ****************************/

#include "em_letimer.h"
#include <stdbool.h>
#include "em_device.h"
#include "gpio.h"
#include "temp_sensor.h"
#include "native_gecko.h"
#include "adc_prs.h"
#include "pulse_adc.h"
#include "graphics.h"

#define MIN_EM_MODE		0
/* Macros for duty cycle and led on-period in seconds  */



#define DUTY_CYCLE_PERIOD_SECONDS		(5)

#define LED0_ON_PERIOD_SECONDS			(0.01)
#define POR_PERIOD_SECONDS				(0.08)

/* Macros for setting LETIMER COMP0 and COMP1 register  */
#define ULFRCO_COUNT					((uint16_t)0x3E8)
#define LFXO_COUNT						((uint16_t)0x7FFF)
#define LETIMER0_COMP0					(0)
#define LETIMER0_COMP1					(1)


#define TEMP_CHECK_EVENT				(23)
#define POR_WAIT_EVENT					(25)
#define PULSE_CHECK_EVENT				(30)


/************FuNCTION Prototypes *************/

extern volatile uint8_t sched_flag;
extern volatile uint16_t pulse_flag;
//uint16_t timer_check;


void LETIMER0_setup(void);
void LETIMER0_SetPrescaler(void);
void LETIMER0_SetCompareVal(void);
void LETIMER0_enable(void);
void LETIMER0_disable(void);
void LETIMER0_IRQHandler(void);


#endif /* LETIMER_H_ */
