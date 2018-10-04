/*
 * sleep.h
 *
 *  Created on: 31-Jan-2018
 *      Author: defaultuser0
 */

#ifndef SLEEP_H_
#define SLEEP_H_

#include "em_core.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_letimer.h"
#include "em_device.h"
#include <stdbool.h>

extern uint8_t sleepBlockCounter[4];

typedef enum{
	EM0,
	EM1,
	EM2,
	EM3,
	EM4
}sleep_enum;

void blockSleepMode(sleep_enum minMode);
void unblockSleepMode(sleep_enum minMode);
void sleep(void);

#endif /* SLEEP_H_ */
