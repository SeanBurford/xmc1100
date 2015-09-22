// Blink two LEDs using the XMC1100 systick timer IRQ.

#include "xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"

static void blink(void) {
	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	setPin(1, 0);

	// 1Hz (8MHz / 8M)
	systickEnable(8000000 - 1);
}

int main() {
	scuPostReset(CLKCR_M8_P8);
        enable_interrupts();
	blink();
        while(1) {
                asm("wfi");
        }
	return 0;
}

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	togglePinP1(0);
	togglePinP1(1);
}

