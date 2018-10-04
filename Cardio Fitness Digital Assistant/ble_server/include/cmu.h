
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

#ifndef SRC_CMU_H_
#define SRC_CMU_H_

#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"

#define MIN_EM_MODE		0

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Blue Gecko Clock Management Unit initialization code
 *
 * @details
 *   The CMU needs to be set up in order to provide clock cycles to the MCU core
 *	 and other peripherals. The CMU oscillator and cloack mode has to be selected
 *	 for this purpose.
 *
 * @param none
 *
 * @return void
 *
 ******************************************************************************/
void cmu_init(void);

void cmu_init2(void);

#endif
