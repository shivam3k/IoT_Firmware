

#include "adc_ldma.h"

void ADC_LDMA_Config(void){

	//dmaDataIncNone;

	//CMU_ClockEnable(cmuClock_LDMA,true);

	LDMA_Init_t init = LDMA_INIT_DEFAULT;
	LDMA_Init( &init );

	LDMA->CH[0].CTRL &=~(LDMA_CH_CTRL_BYTESWAP);
	LDMA->CH[0].CTRL &=~(LDMA_CH_CTRL_DSTMODE);
	LDMA->CH[0].CTRL &=~(LDMA_CH_CTRL_SRCMODE);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_DSTINC_DEFAULT);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_SRCINC_DEFAULT);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_SIZE_WORD);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_DECLOOPCNT_DEFAULT);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_REQMODE_ALL);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_BLOCKSIZE_UNIT128);//
	LDMA->CH[0].CTRL |= (LDMA_XFERCNT_2048);
	LDMA->CH[0].CTRL |= (LDMA_CH_CTRL_STRUCTTYPE_TRANSFER);

	LDMA->CH[0].SRC = (uint32_t)(&ADC0->SINGLEDATA);
	LDMA->CH[0].DST = (uint32_t)(&buffer_adc);
	LDMA->CH[0].REQSEL |= ((0x00000001UL) << 19);
	//LDMA->CH[0].REQSEL |= ((0x00000008UL) << 0);

	//LDMA->CH[0].REQSEL = ((0x00000008UL) << 16);

	LDMA->IFC |= ((0x00000001UL) << 0);
	LDMA->IFC = 0xFFFFFFFFU;
	LDMA->IEN |= ((0x00000001UL) << 0);
	//LDMA->IEN = LDMA_IEN_ERROR;


	NVIC_EnableIRQ(LDMA_IRQn);

}


#if 0

	void LDMA_IRQHandler(void){

		 uint32_t pending;

		  /* Read and clear interrupt source */
		  pending = LDMA->IFC;

		  /* Check for LDMA error */
		  if ( pending & LDMA_IF_ERROR )
		  {
			/* Loop here to enable the debugger to see what has happened */
			while (1)
			  ;
		  }
		  else{

			  LDMA->IFC |= ((0x00000001UL) << 0);
			  sched_flag = PULSE_CALC_EVENT;
			  GPIO_PinOutSet(LED0_port, LED0_pin);
		  }
	}
#endif


void ADC_LDMA_Enable(void){

	CRYOTIMER_Enable(true);
	LDMA->CHEN |= ((0x00000001UL)<<0);

}


