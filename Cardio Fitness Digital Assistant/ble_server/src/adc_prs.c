
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

/****************************************************************************************
 * @reference - Certain sections of this code have been derived from the ldma exmaple code
 * 				for peripheral to memory transfer.
 ****************************************************************************************/


#include "adc_prs.h"

//extern uint8_t BLE_CONNECT;

/****************************************************************************
 * @brief Setup CRYOTIMER as PRS source to trigger ADC
 *****************************************************************************/
void Cryo_ADC_Trigger_Init(void)
{
  CRYOTIMER_Init_TypeDef cryotimerInit = CRYOTIMER_INIT_DEFAULT;

  /* Enable LFRCO for CRYOTIMER */
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

  /* Trigger period = 16 x 1/32768 = 512 Hz */
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);
  cryotimerInit.enable = false;
  cryotimerInit.period = cryotimerPeriod_128;
  CRYOTIMER_Init(&cryotimerInit);

  /* Output to PRS channel */
  CMU_ClockEnable(cmuClock_PRS, true);
  PRS_SourceSignalSet(ADC_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_CRYOTIMER, PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD, prsEdgePos);
}

/****************************************************************************
 * @brief Initialize ADC for single conversion
 *****************************************************************************/
void LDMA_ADC_init(void)
{
  ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  CMU_ClockEnable(cmuClock_ADC0, true);

  /* Initialize common settings for single conversion */
  init.timebase = ADC_TimebaseCalc(0);				// Calculate timebase value in order to get a timebase
  init.prescale = 0;
  ADC_Init(ADC0, &init);

  singleInit.reference = adcRef5V;
  singleInit.acqTime = adcAcqTime1;
  singleInit.prsEnable = true;						// enable Peripheral Reflex System
  singleInit.posSel = ADC_INPUT_PORT;				// Select channel 8 on bus 3x
  singleInit.negSel = adcNegSelVSS;
  ADC_InitSingle(ADC0, &singleInit);				// Initialize ADC

  /* Set data valid level to trigger DMA */
  ADC0->SINGLECTRLX |= (ADC_FIFO_DEPTH - 1) << _ADC_SINGLECTRLX_DVL_SHIFT;

}

/*****************************************************************************
 * @brief
 *   LDMA IRQ handler - this function sends external signal to gecko ble stack to
 *   call the pulse processing function when dma transfer is done.
 ******************************************************************************/

void LDMA_IRQHandler( void )
{
	uint32_t pending;
	/* Disable all interrupts */
	CORE_ATOMIC_IRQ_DISABLE();

  /* Read and clear interrupt source */
  pending = LDMA->IFC;

  /* Check for LDMA error */
  if ( pending & LDMA_IF_ERROR )
  {
    /* Loop here to enable the debugger to see what has happened */
    while (1);
  }
  else{
	  /* Check for DMA transfer */
	  if(LDMA_TransferDone(USE_DMA_CHANNEL)){
		  	  /* Clear DONE Bit Field in IFC register*/
	  	  	  LDMA->IFC |= ((0x00000011UL) << 0);
	  	  	  /* Send external signal to gecko ble stack */
	  	  	  gecko_external_signal(EXT_SIGNAL_PULSE_CAPTURE);
	  }
  }

 /* Enable all interrupts */
 CORE_ATOMIC_IRQ_ENABLE();

}

/*******************************************************************************
 * @brief
 *   Initialize the LDMA controller.
 ******************************************************************************/
void LDMA_init(void)
{
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA->IEN |= ((0x00000011UL)<<0);				// Enable DONE Interrupt
  LDMA_Init( &init );							// Initialize LDMA
}

/*****************************************************************************
 * @brief
 *   Single Descriptor Looped Transfer.
 ******************************************************************************/
void LDMA_loop(void)
{
  /* Fill buffer with dummy value */
  for (uint16_t i=0; i<ADC_BUFFER_SIZE; i++){
	  adcBuffer[i] = ADC_DUMMY_VALUE;
  }

  /* Use looped peripheral transfer configuration macro */
  LDMA_TransferCfg_t periTransferTx =
        LDMA_TRANSFER_CFG_PERIPHERAL_LOOP(ldmaPeripheralSignal_ADC0_SINGLE, LOOP_REPEAT);

  /* Use LINK descriptor macro for initialization and looping */
  LDMA_Descriptor_t xfer[] =
  {
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&ADC0->SINGLEDATA, &adcBuffer, ADC_FIFO_DEPTH, 1),
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&ADC0->SINGLEDATA, ADC_DST_ADDR_OFFSET, ADC_FIFO_DEPTH, 0),
  };

  descLink0[0] = xfer[0];
  descLink0[1] = xfer[1];

  /* First descriptor to initialize transfer */
  descLink0[0].xfer.doneIfs = 0;
  descLink0[0].xfer.blockSize = ldmaCtrlBlockSizeUnit1;
  descLink0[0].xfer.ignoreSrec = 1;
  descLink0[0].xfer.reqMode = ldmaCtrlReqModeAll;
  descLink0[0].xfer.size = ldmaCtrlSizeWord;

  /* Second descriptor for single descriptor looping */
  descLink0[1].xfer.decLoopCnt = 1;
  descLink0[1].xfer.doneIfs = 0;
  descLink0[1].xfer.blockSize = ldmaCtrlBlockSizeUnit1;
  descLink0[1].xfer.ignoreSrec = 1;
  descLink0[1].xfer.reqMode = ldmaCtrlReqModeAll;
  descLink0[1].xfer.size = ldmaCtrlSizeWord;

  /* Use relative addressing to keep destination address, stop after looping */
  descLink0[1].xfer.dstAddrMode = ldmaCtrlSrcAddrModeRel;
  descLink0[1].xfer.link = 0;

  /* Start LDMA Data Transfer using Channel 0 */
  LDMA_StartTransfer(USE_DMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink0);

  /* Start cryotimer to trigger ADC, wait LDMA finish */
  CRYOTIMER_Enable(true);

}

