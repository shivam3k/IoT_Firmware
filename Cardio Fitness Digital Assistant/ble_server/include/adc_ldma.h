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

#ifndef SRC_ADC_LDMA_H_
#define SRC_ADC_LDMA_H_

#include "adc_prs.h"

#define LDMA_XFERCNT_2048			(0x00000080UL << 4)
#define ADC_BUFFER_SIZE_TEMP				(20)

uint32_t buffer_adc[ADC_BUFFER_SIZE_TEMP];
extern volatile uint8_t sched_flag;

#define PULSE_CALC_EVENT					(67)


void ADC_LDMA_Config();
//void LDMA_IRQHandler(void);
void ADC_LDMA_Enable(void);
#endif /* ADC_LDMA_H_ */
