
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
// defined files
//***********************************************************************************
#include "gpio.h"

//extern volatile uint8_t button_flag;
//extern volatile uint32_t step_count = 0;

void gpio_init(void){

	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_PinModeSet(ADC0_port, ADC0_pin, gpioModeDisabled, ADC0_default);
	GPIO_PinModeSet(ADC_EN_port, ADC_EN_pin, gpioModePushPull, ADC_EN_default);

	GPIO_PinModeSet(BTN_port, BTN_PB1_pin, gpioModeInput, 1);
	GPIO_PinModeSet(BTN_port, BTN_PB0_pin, gpioModeInput, 1);

	GPIO_PinModeSet(ACC_INT1_port, ACC_INT1_pin, gpioModePushPull, ACC_INT1_default);

	GPIO_ExtIntConfig(ACC_INT1_port, ACC_INT1_pin, 3, false, true, true);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

}


void GPIO_ODD_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE();

	uint32_t gpioInt;

	gpioInt = GPIO->IF;

	if(gpioInt & ACC_INT_SOURCE){

		GPIO->IFC |= ACC_INT_SOURCE;

		gecko_external_signal(EXT_SIGNAL_ACCELEROMETER_INT);
	}

    CORE_ATOMIC_IRQ_ENABLE();
}

void GPIO_EVEN_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE();

    GPIO_IntClear(GPIO_IntGet());
    button_flag = 0;
    button_flag = PASSKEY_YES_EVENT;
    GPIO_IntEnable(BTN_port);

    CORE_ATOMIC_IRQ_ENABLE();
}
