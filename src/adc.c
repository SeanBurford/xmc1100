#include "xmc1100.h"
#include "adc.h"
#include "gpio.h"
#include "scu.h"

int enableADCPin(const unsigned int port, const unsigned int pin) {
	if (port != 2) {
		// Only port 2 of the XMC1100 has A/D functionality.
		return 1;
	}
        clearPin(port, pin);

	// XXX
}

void disableADCPin(const unsigned int port, const unsigned int pin) {
	if (port != 2) {
		// Only port 2 of the XMC1100 has A/D functionality.
		return 1;
	}
	// XXX
}

void adcEnable(void) {
	// Clear gating of ADC in SCU_CGATCLR0
	scuUngatePeripheralClock(CGATCLR0_VADC);

	// XXX
}

void adcDisable(void) {
	// XXX

	// Gate the ADC in SCU_CGATCLR0
	scuGatePeripheralClock(CGATCLR0_VADC);
}

