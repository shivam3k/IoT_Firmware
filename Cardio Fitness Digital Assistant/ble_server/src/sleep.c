
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

#include "sleep.h"


//sleep count array to determine the minimum energy mode
uint8_t sleepBlockCounter[4] = {0,0,0,0};


/***************************************************************************//**
 * @brief
 *   block the microcontroller from sleeping below a certain mode
 *
 * @details
 *   This function blocks the microcontroller from sleeping below a certain mode.
 *
 * @param minMode enum
 *
 * @return void
 *
 ******************************************************************************/

void blockSleepMode(sleep_enum minMode){
	//disable interrupts globally
	CORE_ATOMIC_IRQ_DISABLE();
	// increment sleep count for the  minimum energy mode
	sleepBlockCounter[minMode]++;
	//enable interrupts globally
	CORE_ATOMIC_IRQ_ENABLE();
}

/***************************************************************************//**
 * @brief
 *   unblock the microcontroller from sleeping below a certain mode
 *
 * @details
 *   This function unblocks the microcontroller from sleeping below a certain mode.
 *
 * @param minMode enum
 *
 * @return void
 *
 ******************************************************************************/
void unblockSleepMode(sleep_enum minMode){
	//disable interrupts globally
	CORE_ATOMIC_IRQ_DISABLE();
	if(sleepBlockCounter[minMode]>0){
	// decrement sleep count for the  minimum energy mode
	sleepBlockCounter[minMode]--;}
	//enable interrupts globally
	CORE_ATOMIC_IRQ_ENABLE();
}

/***************************************************************************//**
 * @brief
 *   energy mode sleep routine
 *
 * @details
 *   This function determines which energy mode has to be enabled
 *
 * @param minMode enum
 *
 * @return void
 *
 ******************************************************************************/

void sleep(void){
	if(sleepBlockCounter[0] > 0){ // stay in EM0
		return;}
	else if(sleepBlockCounter[1] > 0){  // EM2 is blocked, so go into EM1
		EMU_EnterEM1();}
	else if(sleepBlockCounter[2] > 0){	// EM3 is blocked, so go into EM2
		EMU_EnterEM2(true);
		}
	else{
		EMU_EnterEM3(true);				// EM4 is blocked, so go into EM3
	}
	return;
}
