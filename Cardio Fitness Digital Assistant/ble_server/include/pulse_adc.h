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

#ifndef SRC_PULSE_ADC_H_
#define SRC_PULSE_ADC_H_


#include "em_adc.h"
#include "em_core.h"
#include "gpio.h"
#include "sleep.h"
#include <stdbool.h>
#include "graphics.h"
#include "em_cmu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "letimer.h"
#include <string.h>
#include "adc_prs.h"

#define OPERATING_FREQ 						(1000)
#define NO_OF_SAMPLES 						(200)
#define ADC_CLOCK							(11000000)
#define ADC_12BIT_MAX						(4096)
#define ADC_REF_VOLTAGE						(5)

volatile int BPM;                   	  // int that holds raw Analog in 0. updated every 2mS
volatile uint32_t Signal;                 // holds the incoming raw data
volatile int IBI;            			  // int that holds the time interval between beats! Must be seeded!
volatile bool Pulse;     				  // "True" when User's live heartbeat is detected. "False" when not a "live beat".

volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter;     // used to determine pulse timing
volatile unsigned long lastBeatTime;      // used to find IBI
volatile uint32_t P;                      // used to find peak in pulse wave, seeded
volatile uint32_t T;                      // used to find trough in pulse wave, seeded
volatile uint32_t thresh;                 // used to find instant moment of heart beat, seeded
volatile int amp;                   	  // used to hold amplitude of pulse waveform, seeded
volatile bool firstBeat;        		  // used to seed rate array so we startup with reasonable BPM
volatile bool secondBeat;
volatile uint16_t pulse_count;

char pulse_buffer1[10];
char pulse_buffer2[10];
char pulse_buffer3[10];
char pulse_buffer4[10];
char pulse_buffer5[10];
char pulse_buffer6[10];


void Pulse_Init(void);
uint32_t Pulse_Read(void);
void ADC0_Reset(void);
int Pulse_Processing(void);
int Pulse_Processing_dma(void);

#endif /* PULSE_ADC_H_ */
