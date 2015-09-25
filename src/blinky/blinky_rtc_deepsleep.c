// Blink two LEDs using the XMC1100 RTC between CPU deep sleeps.
//
// Functions in section .stext are loaded into SRAM by peripherals/init.c 
// which means flash does not need to be turned on for their execution provided
// we're careful to only call other functions in SRAM from them.

#include "xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/rtc.h"

// This loop might become active from deep sleep so we put it in .stext.
void __attribute__((section(".stext"))) sleep(
    const unsigned int disable_flash,
    const unsigned int deep_sleep) {
	if (disable_flash) {
		// Turn off NVM_ON.
		// To reverse in gdb:
		// (gdb) set *0x40050008 = 0x00009000
		if (NVMCONF & BIT15) {
			NVMCONF ^= BIT15;
		}
		// Flash will power off in power save and come back on
		// when we exit power save.  This is slower.
		// SCU_PWRSVCR = BIT0;
	}
	if (deep_sleep) {
		// Enable deep sleep in the system control register.
		// BIT2: Low power sleep mode.
		// BIT4: All interrupts (including disabled) wake processor.
		SCS_SCR |= BIT2 | BIT4;
	}

	while (1) {
		asm("wfi");
	}
} 

// Interrupt handler for the RTC.
// This should become active from deep sleep so we put it in .stext.
void __attribute__((interrupt("IRQ"), section(".stext"))) SCU_SR1(void) {
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

// Insurance in case we mess up.
void __attribute__((interrupt("IRQ"), section(".stext"))) hardfaultHandler(void)
{
        // Turn flash on (if it's off) and wait for it to become active.
        // Unnecessary unless we've used NVMCONF to turn off flash.
        NVMCONF |= BIT15;  // Turn on flash.
        while (NVMSTATUS & BIT1) {
                // BIT1: SLEEP mode is active.
        }
        while(1) {
		asm("wfi");
        }
}

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
	sleep(1, 1);
	return 0;
}


