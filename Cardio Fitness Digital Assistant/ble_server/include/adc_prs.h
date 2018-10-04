/*****************************************************************************
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



#ifndef SRC_ADC_PRS_H_
#define SRC_ADC_PRS_H_

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_prs.h"
#include "em_ldma.h"
#include "em_cryotimer.h"
#include "graphics.h"
#include "gpio.h"
#include "adc_ldma.h"
#include "pulse_adc.h"
#include "letimer.h"

extern volatile uint8_t sched_flag;
extern volatile uint16_t pulse_flag;

/************************************** Macros *********************************************/

/********************************* External signal macros **********************************/

#define EXT_SIGNAL_ACCELEROMETER_INT		 (0x01)
#define EXT_SIGNAL_PULSE_PROCESSING		 	 (0x02)
#define EXT_SIGNAL_PULSE_CAPTURE			 (0x03)

/* DMA channel used for the examples */
#define USE_DMA_CHANNEL         0
#define USE_DMA_CH_MASK         (1 << USE_DMA_CHANNEL)

/* Memory to memory transfer buffer size and constant for GPIO PRS */
#define BUFFER_SIZE             127
#define TRANSFER_SIZE           (BUFFER_SIZE - 1)
#define GPIO_PRS_CHANNEL        1

/* ADC buffer size and constants for ADC setup */

#define ADC_BUFFER_SIZE         1024
#define ADC_CLOCK_1             10000000
#define ADC_INPUT_PORT          adcPosSelAPORT3XCH8
#define ADC_PRS_CHANNEL         0
#define ADC_FIFO_DEPTH          4
#define ADC_DST_ADDR_OFFSET     0
#define ADC_DUMMY_VALUE         10000000

/* Constant for loop transfer */
#define LOOP_REPEAT             254

/* Buffer for single descriptor looped transfer */
uint32_t adcBuffer[ADC_BUFFER_SIZE];

/* Descriptor linked list for LDMA transfer */
LDMA_Descriptor_t descLink0[2];

extern volatile uint16_t p_cnt_1;


/********************* LDMA AND CRYOTIMER FUNCTIONS ****************/

void Cryo_ADC_Trigger_Init(void);
void LDMA_ADC_init(void);
void LDMA_IRQHandler(void);
void LDMA_init(void);
void LDMA_loop(void);

/********************************************************************/


#endif /* ADC_PRS_H_ */
