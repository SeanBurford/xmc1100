#include "peripherals/xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/scu.h"

static void blinkPins(void) {
	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	setPin(1, 0);

	while(1) {
		unsigned int count = 0;
		for (count = 500000; count > 0; count--) {
			;
		}
		togglePinP1(0);
		togglePinP1(1);
	}
}

int main() {
	scuPostReset(CLKCR_M8_P8);
        enable_interrupts();
	blinkPins();
	return 0;
}
