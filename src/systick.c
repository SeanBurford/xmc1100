#include "xmc1100.h"
#include "gpio.h"
#include "systick.h"

void systickEnable(void) {
	// Set the reload register (timebase in effect)
	// Clock = 64MHz so 8000000-1 is 8 systicks/second.
	SYST_RVR = 8000000 - 1; // generate 1/8 second time base
	SYST_CVR = 5; // Start the counter at a value close to zero
	// Use processor clock, enable interrupt request and enable counter
	SYST_CSR |= BIT0 | BIT1 | BIT2;
}

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	// Toggle LED P1.1.
	togglePinP1(1);
}
