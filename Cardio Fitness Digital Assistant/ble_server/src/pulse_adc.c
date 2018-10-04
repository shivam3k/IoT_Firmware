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

#include "pulse_adc.h"


void Pulse_Init(void){

	ADC_Init_TypeDef Init =
	{
			  .ovsRateSel = adcOvsRateSel2,
			  .prescale = OPERATING_FREQ/(NO_OF_SAMPLES + 1),
			  .warmUpMode = adcWarmupNormal,
	          .timebase = ADC_TimebaseCalc(0),
			  .tailgate = false,
			  .em2ClockConfig = adcEm2ClockOnDemand
	};

	ADC_InitSingle_TypeDef singleInit =
	{
			  .acqTime = adcAcqTime32,
			  .reference = adcRef5V,
			  .resolution = adcRes12Bit,
			  .posSel = ADC_INPUT_PORT,
			  .negSel=adcNegSelVSS,
			  .diff = false,
			  .prsEnable = false,
			  .leftAdjust = false ,
			  .rep = true,
			  .singleDmaEm2Wu = false,
			  .fifoOverwrite = true
	};

	ADC_Init(ADC0, &Init);
	ADC_InitSingle(ADC0, &singleInit);

	ADC0->BIASPROG |=  ADC_BIASPROG_GPBIASACC | ADC_BIASPROG_ADCBIASPROG_SCALE32;
	ADC_IntClear(ADC0, ADC_IFC_PROGERR | ADC_IFC_VREFOV | ADC_IFC_SCANCMP | ADC_IFC_SINGLECMP | ADC_IFC_SCANUF | ADC_IFC_SINGLEUF | ADC_IFC_SCANOF | ADC_IFC_SINGLEOF);
	NVIC_DisableIRQ(ADC0_IRQn);
	ADC_Start(ADC0, adcStartSingle);

	IBI = 600;
	Pulse = false;

	sampleCounter = 0;
	lastBeatTime = 0;
	P = 2048;
	T = 2048;
	thresh = 2200;
	amp = 0;
	firstBeat = true;
	secondBeat = false;
}
//#if 0
int Pulse_Processing(void){

	Signal = Pulse_Read();							// read the Pulse Sensor

	  sampleCounter += 20;                         // keep track of the time in mS with this variable
	  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

	    //  find the peak and trough of the pulse wave
	  if(Signal < thresh && N > (IBI/5)*3){       // avoid noise by waiting 60% of last IBI
	    if (Signal < T){                        // T is the trough
	      T = Signal;                         // keep track of lowest point in pulse wave
	    }
	  }

	  if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
	    P = Signal;                             // P is the peak
	  }                                        // keep track of highest point in pulse wave


	  if (N > 250){                                   // condition to avoid high frequency noise
	    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
	      Pulse = true;                               // set the Pulse flag when we think there is a pulse
	      GPIO_PinOutSet(LED1_port, LED1_pin);             // turn on LED1
	      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
	      lastBeatTime = sampleCounter;               // keep track of time for next pulse

	      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
	        secondBeat = false;                  // clear secondBeat flag
	        for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
	          rate[i] = IBI;
	        }
	      }

	      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
	        firstBeat = false;                   // clear firstBeat flag
	        secondBeat = true;                   // set the second beat flag
	        //return;                              // IBI value is unreliable so discard it
	      }


	      // keep a running total of the last 10 IBI values
	      long int runningTotal = 0;                  // clear the runningTotal variable

	      for(int i=0; i<=8; i++){                // shift data in the rate array
	        rate[i] = rate[i+1];                  // and drop the oldest IBI value
	        runningTotal += rate[i];              // add up the 9 oldest IBI values
	      }

	      rate[9] = IBI;                          // add the latest IBI to the rate array
	      runningTotal += rate[9];                // add the latest IBI to runningTotal
	      runningTotal /= 10;                     // average the last 10 IBI values
	      BPM = 60000/runningTotal;               // calculating the BPM

	      //if(BPM > 180){BPM = 0;}

	      snprintf(pulse_buffer1, sizeof(pulse_buffer1), "%d", BPM);
	      //snprintf(pulse_buffer2, sizeof(pulse_buffer2), "%ld", runningTotal);
	      snprintf(pulse_buffer3, sizeof(pulse_buffer3), "%ld", Signal);
	      snprintf(pulse_buffer4, sizeof(pulse_buffer4), "%ld", P);
	      //snprintf(pulse_buffer5, sizeof(pulse_buffer5), "%ld", T);
	      //snprintf(pulse_buffer6, sizeof(pulse_buffer6), "%ld", thresh);

	      GRAPHICS_Clear();
	      GRAPHICS_AppendString("BPM - ");
	      GRAPHICS_AppendString(pulse_buffer1);
	     // GRAPHICS_AppendString("RT - ");
	     // GRAPHICS_AppendString(pulse_buffer2);
	      GRAPHICS_AppendString("S,P");
	      GRAPHICS_AppendString(pulse_buffer3);
	      GRAPHICS_AppendString(pulse_buffer4);
	     // GRAPHICS_AppendString(pulse_buffer5);
	     // GRAPHICS_AppendString(pulse_buffer6);
	      GRAPHICS_Update();

	    }
	  }

	  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
		GPIO_PinOutClear(LED1_port, LED1_pin);;            // turn off LED1
	    Pulse = false;                         // reset the Pulse flag so we can do it again
	    amp = P - T;                           // get amplitude of the pulse wave
	    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
	    P = thresh;                            // reset these for next time
	    T = thresh;
	  }

	  if (N > 2500){                           // if 2.5 seconds go by without a beat
	    thresh = 2100;                          // set thresh default
	    P = 2048;                               // set P default
	    T = 2048;                               // set T default
	    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
	    firstBeat = true;
	    secondBeat = false;
	  }

	  return BPM;								//return pulse rate

}

int Pulse_Processing_dma(void){

	firstBeat = true;
	secondBeat = false;
	pulse_count = 0;

	IBI = 600;
	Pulse = false;

	sampleCounter = 0;
	lastBeatTime = 0;
	P = 2048;
	T = 2048;
	thresh = 2100;
	amp = 0;
	firstBeat = true;
	secondBeat = false;
	uint16_t N = 0;

	long int runningTotal = 0;

for(uint16_t p=0;p<ADC_BUFFER_SIZE;p++){

	Signal = adcBuffer[p];							// read the Pulse Sensor

	  sampleCounter += 4;                         // keep track of the time in mS with this variable
	  N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

	    //  find the peak and trough of the pulse wave
	  if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 60% of last IBI
	    if (Signal < T){                        // T is the trough
	      T = Signal;                         // keep track of lowest point in pulse wave
	    }
	  }

	  if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
	    P = Signal;                             // P is the peak
	  }                                        // keep track of highest point in pulse wave


	  if (N > 250){                                   // condition to avoid high frequency noise
	    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
	      Pulse = true;                               // set the Pulse flag when we think there is a pulse
	      //GPIO_PinOutSet(LED1_port, LED1_pin);             // turn on LED1
	      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
	      lastBeatTime = sampleCounter;               // keep track of time for next pulse
		  pulse_count++;


	      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
	        secondBeat = false;		   			// clear secondBeat flag
			rate[pulse_count-2] = IBI;
	        //for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
	        //  rate[i] = IBI;
	       // }
	      }

	      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
	        firstBeat = false;                   // clear firstBeat flag
	        secondBeat = true;                   // set the second beat flag
	                                   // IBI value is unreliable so discard it
	      }


	      // keep a running total of the last 10 IBI values
	      //long int runningTotal = 0;                  // clear the runningTotal variable



	    }
	  }


	  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
		//GPIO_PinOutClear(LED1_port, LED1_pin);;            // turn off LED1
	    Pulse = false;                         // reset the Pulse flag so we can do it again
	    //amp = P - T;                           // get amplitude of the pulse wave
	    //thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
	    //P = thresh;                            // reset these for next time
	    //T = thresh;
	  }

	  if (N > 2500){                           // if 2.5 seconds go by without a beat
	    //thresh = 2200;                          // set thresh default
	    P = 2048;                               // set P default
	    T = 2048;                               // set T default
	    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
	    firstBeat = true;
	    secondBeat = false;
	  }
	}


		for(uint8_t i=0; i<(pulse_count-1); i++){                // shift data in the rate array
	        //rate[i] = rate[i+1];                  // and drop the oldest IBI value
	        runningTotal += rate[i];              // add up the 9 oldest IBI values
	      }

	      //rate[9] = IBI;                          // add the latest IBI to the rate array
	      //runningTotal += rate[9];                // add the latest IBI to runningTotal
	      runningTotal /= (pulse_count-1);                     // average the last 10 IBI values
	      if(runningTotal > 0){
	    	  BPM = 15000/runningTotal;}
	      else{
	    	  BPM = 200000;
	      }// calculating the BPM

	      if(BPM > 150){
	    	  BPM = 0;
	      }

		  snprintf(pulse_buffer1, sizeof(pulse_buffer1), "%d", BPM);
	      //snprintf(pulse_buffer2, sizeof(pulse_buffer2), "%ld", runningTotal);
	      snprintf(pulse_buffer3, sizeof(pulse_buffer3), "%ld", Signal);
	      snprintf(pulse_buffer4, sizeof(pulse_buffer4), "%ld", P);
	      snprintf(pulse_buffer5, sizeof(pulse_buffer5), "%ld", T);
	      //snprintf(pulse_buffer6, sizeof(pulse_buffer6), "%ld", thresh);

	      GRAPHICS_Clear();
	      GRAPHICS_AppendString("BPM - ");
	      GRAPHICS_AppendString(pulse_buffer1);
	      //GRAPHICS_AppendString("RT - ");
	      //GRAPHICS_AppendString(pulse_buffer2);
	      GRAPHICS_AppendString("S,P,T");
	      GRAPHICS_AppendString(pulse_buffer3);
	      GRAPHICS_AppendString(pulse_buffer4);
	      GRAPHICS_AppendString(pulse_buffer5);
	     // GRAPHICS_AppendString(pulse_buffer6);
	      GRAPHICS_Update();

	return BPM;
}


uint32_t Pulse_Read(void){
	uint32_t ADC_data = ADC_DataSingleGet(ADC0);
	    /*Clear FIFO*/
	    ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;
return ADC_data;
}

/**************************************************************************//**
 * @brief Reset ADC registers and disable clocks.
 *****************************************************************************/

void ADC0_Reset(void)
{
 /* Rest ADC registers */
 ADC_Reset(ADC0);
}

