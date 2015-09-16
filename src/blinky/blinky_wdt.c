// Blink two LEDs using the watchdog timer alarm IRQ and WDT reboot.
//
// Normally you would avoid a WDT reboot by calling wdtService() while WDT_TIM
// is between the lower and upper bound.  To demonstrate a WDT reboot I've
// neglected to do that, instead using the warning to toggle the LEDs then
// I blink them both on WDT reset.

#include "peripherals/xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/scu.h"
#include "peripherals/wdt.h"

int main() {
	unsigned int reset_reason = scuPostReset(CLKCR_M8_P8);

	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED

	if (reset_reason & RSTSTAT_WDT) {
		unsigned int i, j;
		// Indicate that a WDT reboot occurred by blinking both LEDs.
		for (i = 0; i < 4; i++) {
			togglePinP1(0);
			togglePinP1(1);
			for (j = 0; j < 50000; j++)
				;
		}
	}

        enable_interrupts();

	togglePinP1(0);
	// Since we're using the prealarm to fire a warning the lower bound
	// never comes into play.
	wdtEnable(0x00007f00, 0x00008000, 1);

        while(1) {
                asm("wfi");
        }
	return 0;
}

// Interrupt handler for the WDT
void __attribute__((interrupt("IRQ"))) SCU_SR1(void) {
	unsigned int wdt_stat = WDT_WDTSTS;
	if (wdt_stat & WDT_ALMS) {
		// We failed to service the wdt. Next timeout will reboot
		// unless we wdtService() within the window.
		togglePinP1(0);
		togglePinP1(1);
		// Clear event bits.
		WDT_WDTCLR = WDT_ALMS;
	}
}
