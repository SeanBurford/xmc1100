// Blink two LEDs using per second periodic event interrupts from the RTC.
// The RTC offers both alarms and periodic events (second, minute ... year).

#include "peripherals/xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/scu.h"
#include "peripherals/rtc.h"

static void blink(void) {
	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	setPin(1, 0);

	// Request periodic (per second) RTC events.
	rtcEnable(2015, 1, 1, 9, 0, 0);
	rtcSetPeriodicEvent(MSKSR_MPSE);
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

// Interrupt handler for the RTC
void __attribute__((interrupt("IRQ"))) SCU_SR1(void) {
	unsigned int stssr = RTC_STSSR;
	if (stssr & MSKSR_MPALL) {
		// Periodic service request
		togglePinP1(0);
		togglePinP1(1);
	}
	if (stssr & MSKSR_MAI) {
		// Alarm
	}
	// Clear event bits.
	RTC_CLRSR = MSKSR_MAI | MSKSR_MPALL;
}
