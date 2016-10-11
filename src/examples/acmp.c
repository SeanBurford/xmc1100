// ACMP example.
//
// When ACMP1 is configured as ACMP_CMP_EN | ACMP_CMP_HYST_ADJ_15
// and GPIO 1.0 is configured as GPIO_OUT_PP_ALT6 (ACMP1.OUT) this
// code will cause the P1.0 LED to follow P2.11 when:
//   P2.7 IN_FLOAT ACMP1.INP is connected to P2.11 via a 390 ohm resistor
//   P2.6 IN_FLOAT ACMP1.INN is connected to VDD (3.3v) via a 390 ohm resistor

#include "xmc1100.h"
#include "peripherals/acmp.h"
#include "peripherals/eru.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	// P1.1 indicates that systick fired.
	togglePinP1(1);
	// togglePinP2(6);
	// togglePinP2(7);

	togglePinP2(11); // ACMP.Vref
	// togglePinP2(6);  // ACMP1.INN
	// togglePinP2(7);  // ACMP1.INP
}

int main()
{
	// Changing this clock speed also requires a change to the argument
	// to systickEnable().
	scuPostReset(CLKCR_M8_P8);

	// Configure GPIO
	enablePin(1, 0, GPIO_OUT_PP_ALT6);  // LED is ACMP1.OUT
	enablePin(1, 1, GPIO_OUT_PP);       // LED
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);     // P2.2 is the debug serial input
	// enablePin(2, 8, GPIO_IN_FLOAT);     // P2.8 ACMP0.INN (Vref/2)
	// enablePin(2, 9, GPIO_OUT_PP);       // P2.9 ACMP0.INP
	enablePin(2, 6, GPIO_IN_FLOAT);     // P2.6 ACMP1.INN
	enablePin(2, 7, GPIO_IN_FLOAT);     // P2.7 ACMP1.INP (Vref/2)
	enablePin(2, 11, GPIO_OUT_PP);      // P2.11 ACMP.Vref
	clearPin(1, 1);

	// 1Hz (8MHz / 8M)
	systickEnable(8000000 - 1);

	// Configure ACMP
	acmpEnable();
	// ACMP1 enabled with 15mv hysteresis and INP is Vref/2
	acmpConfigure(1, ACMP_CMP_EN |
	                 // ACMP_CMP1_DIV_EN |
	                 ACMP_CMP_HYST_ADJ_15);
	// ACMP0 enabled with 15mv hysteresis and INN is Vref/2
	//acmpConfigure(0, ACMP_CMP_EN |
	//                 ACMP_CMP0_SEL |
	//                 ACMP_CMP_HYST_ADJ_15);

	// Turn on Vref
	clearPin(2, 11);  // Vref
	// Turn off ACMP inputs
	// clearPin(2, 6);  // ACMP1.INN
	// clearPin(2, 7);  // ACMP1.INP

	enable_interrupts();

	while(1)
	{
		asm("wfi");
	}
	return 0;
}
