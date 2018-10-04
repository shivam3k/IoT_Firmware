/*
 * letimer.c
 *
 *  Created on: 31-Jan-2018
 *      Author: Shivam Khandelwal
 */


/***************************************************************************//**
 * @Author Shivam Khandelwal
 * @Professor Keith Graham
 * @IOT 2018 Spring
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


#include "letimer.h"
#include <stdbool.h>
#include "em_letimer.h"
#include "sleep.h"
#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"

/***************************************************************************//**
 * @brief
 *   LETIMER presecaler set funtion
 *
 * @details
 *   This function determines the prescaler for the LFA clock tree depending on
 *   the duty cycle and energy mode.
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/
volatile uint8_t sched_flag = 0;
volatile uint16_t pulse_flag = 0;

extern uint8_t BLE_CONNECT;

void LETIMER0_SetPrescaler(void){

	uint8_t base_prescaler = 0;
	uint16_t power = 1;


	while(power < DUTY_CYCLE_PERIOD_SECONDS){		//loop compares power with duty cycle to determine prescaler
			power <<= 2;
			base_prescaler++;							// base_prescaler increases in each iteration
		}

		 CMU->LFAPRESC0 = base_prescaler + 1;			// writing prescaler value to lfapresc0 register.

}
/***************************************************************************//**
 * @brief
 *   LETIMER compare set function
 *
 * @details
 *   This function determines the compare values to set the COMP0 AND COMP1 registers.
 *   It also initializes the low-energy timer depending upon the energy  mode.
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void LETIMER0_SetCompareVal(void){


	uint32_t comp0val;
	//uint32_t comp1val;
	uint32_t base_prescaler = CMU->LFAPRESC0;
	uint32_t prescaler = 1;

#if (MIN_EM_MODE == 3)

	comp0val = (((ULFRCO_COUNT)/(prescaler << base_prescaler))*(DUTY_CYCLE_PERIOD_SECONDS));
	//comp1val = comp0val - (((ULFRCO_COUNT)/(prescaler << base_prescaler))*(POR_PERIOD_SECONDS));

	/* compare set if EM0 -EM2 */

#else
	// Determining values to write to COMP0 and COMP1 registers
	comp0val = (((LFXO_COUNT)/(prescaler << base_prescaler))*(DUTY_CYCLE_PERIOD_SECONDS));
	//comp1val = comp0val - (((LFXO_COUNT)/(prescaler << base_prescaler))*(POR_PERIOD_SECONDS));

#endif
	// Writing values to COMP0 and COMP1 registers
	LETIMER0->COMP0 = comp0val;
	//LETIMER0->COMP1 = comp1val;

// wait for LF Oscillators to synchronize
	while((LETIMER0->SYNCBUSY));
}

/***************************************************************************//**
 * @brief
 *   LETIMER setup configuration function
 *
 * @details
 *   This function sets the required registers and bits to LETIMER to initialize the
 *   LETIMER
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void LETIMER0_setup(void){


	LETIMER0->CTRL |= LETIMER_CTRL_COMP0TOP;
	LETIMER0_SetPrescaler();
	LETIMER0_SetCompareVal();

	//Clear any pending  interrupts
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);

	//Clear IEN Register

	LETIMER0->IEN &= ~((LETIMER_IEN_COMP0));
	LETIMER0->IEN |= ((LETIMER_IEN_COMP0));

	//LETIMER0->IEN &= ~((LETIMER_IEN_COMP0)|(LETIMER_IEN_COMP1));
	//LETIMER0->IEN |= ((LETIMER_IEN_COMP0)|(LETIMER_IEN_COMP1));

	//enable LETIMER0 Interrupt

	NVIC_EnableIRQ(LETIMER0_IRQn);

}

/***************************************************************************//**
 * @brief
 *   LETIMER enable function
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/
void LETIMER0_enable(void){
	LETIMER0->CMD = LETIMER_CMD_START;
	//timer_check = 0;
}

/***************************************************************************//**
 * @brief
 *   LETIMER disable function
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void LETIMER0_disable(void){
	LETIMER0->CMD |= LETIMER_CMD_STOP;
}

/***************************************************************************//**
 * @brief
 *   LETIMER interrupt handler function
 *
 * @details
 *   This function handles the COMP0, COMP1 and underflow interrputs and controls
 *   LED0 blinking
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/

void LETIMER0_IRQHandler(void){

	uint32_t irq_flag;

	CORE_ATOMIC_IRQ_DISABLE();

	irq_flag = LETIMER0->IF;
	LETIMER0->IFC = irq_flag;

	// if interrupt due to COMP0 match
	if(irq_flag & LETIMER_IF_COMP0){
		//clear COMP0 bit in IFC register
		LETIMER0->IFC |= (LETIMER_IFC_COMP0);
		sched_flag = TEMP_CHECK_EVENT;
		gecko_external_signal(EXT_SIGNAL_PULSE_PROCESSING);
	}
	/*
	// if interrupt due to COMP1 match
	if(irq_flag & LETIMER_IF_COMP1){
			//clear COMP1 bit in IFC register
		LETIMER0->IFC |= (LETIMER_IFC_COMP1);
		sched_flag =TEMP_CHECK_EVENT;
	}
	 */

	//gecko_cmd_le_connection_get_rssi(BLE_CONNECT);

	CORE_ATOMIC_IRQ_ENABLE();
}



