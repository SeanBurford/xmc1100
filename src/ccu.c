#include "xmc1100.h"
#include "ccu.h"
#include "nvic.h"

// Slice 0: 40kHz 50% PWM.
//   Event 1: Start signal from SCU.
//   P0.6: open drain PWM output.
//   P1.0: push pull PWM output (LED).
// Slice 1: Capture.
//   Event 0: SR0 (IRQ 21) on capture.
//   Event 1: Start signal from SCU.
//   P0.7 pull up input to trigger capture.

void ccuStartSlices(void) {
	CCU4_CC40TCSET = BIT0; // set timer slice 0 run bit.
	CCU4_CC41TCSET = BIT0; // set timer slice 1 run bit.
	// Remove the (default) idle mode from the timer slices (17.7.1).
	CCU4_GIDLC = BIT1 | BIT0;
	// Start CCU40 timers that have been configured (INS and CMC registers)
	// to use SCU.GSC40 as a start event.
	SCU_CCUCON = BIT0;
}

void ccuStopSlices(void) {
	CCU4_CC40TCCLR = BIT0 | BIT1;  // Clear slice 0 run bit(0) and timer(1).
	CCU4_CC41TCCLR = BIT0 | BIT1;  // Clear slice 1 run bit(0) and timer(1).
	CCU4_CC42TCCLR = BIT0 | BIT1;  // Clear slice 2 run bit(0) and timer(1).
	CCU4_CC43TCCLR = BIT0 | BIT1;  // Clear slice 3 run bit(0) and timer(1).
	CCU4_GIDLS = BIT3 | BIT2 | BIT1 | BIT0;  // put all slices into idle.
}

void ccuEnable(void) {
	// CCU4 clock is SCU fPCLK.
	// Disable gating of the CCU40 clock.
	// Also controls ADC, USIC, WDT, RTC.
	SCU_PASSWD = 0x00C0;
	SCU_CGATCLR0 = BIT2;  // Enable CCU4 clock.
	SCU_PASSWD = 0x00C3;
	while (SCU_CLKCR & (BIT30 | BIT31)); // wait for vccp to stabilise.

	// Deassert synchronised start signal.
	SCU_CCUCON = 0;
	ccuStopSlices();

	// Enable prescaler block (SPRB = 1).  17.7.1
	// Writing a BIT8 enables the prescaler run bit for the CCU.
	CCU4_GIDLC = BIT8;
	// Configure the global CCU4 register.  17.7.1
	// Request shadow transfer of period and compare registers.
	// Module clock is prescaler clock.
	// Prescaler cleared by software only.
	CCU4_GCTRL = 0;
}

void ccuConfigureSlice0(void) {
	// Prescaler frequency is Fccu/(PSC^2). 0=/1, 1 = /2, 2=/4, 3=/8 etc.
	CCU4_CC40PSC = 4;  // 64 / 16 = 4MHz
	// Set CAPCOM timer to compare mode (CMOD=0, default) and timer counting
	// mode to edge aligned.
        // BIT2: Shadow transfer on clear.
	CCU4_CC40TC = BIT2;
	// Set the period match and compare level shadow registers.
	// Request transfer of shadow registers to the normal registers.
	// Using PRS blocks out C2V and C3V capture registers.
	// Using CRS blocks out C0V and C1V capture registers.
	CCU4_CC40PRS = 99;  // 40kHz
	CCU4_CC40CRS = 50;
	CCU4_GCSS = BIT0;  // Request transfer of PRS and CRS (slice 0)
	// Set external output to passive low external high.
	// output = value ^ PSL
	CCU4_CC40PSL = 0;  //Set PSL bit to request passive low.

	// Event 1: active high, rising edge, input I (SCU)
	CCU4_CC40INS = (1 << 18) | (8 << 4);
	CCU4_CC40CMC = 2;  // Start on event 1
	CCU4_CC40INTE = 0;  // No interrupts
}

void ccuConfigureSlice1(void) {
	CCU4_CC41PSC = 4;  // 64 / 16 = 4MHz
	CCU4_CC41TC = 0;
	CCU4_CC41PSL = 0;  // Set PSL bit to request passive low.

	CCU4_CC41PRS = 0xFFFF;
	CCU4_CC41CRS = 0xFFFF;
	CCU4_GCSS = BIT4;  // Request transfer of PRS and CRS (slice 1)

	// To select an external capture signal, one should map one of the
	// events (output of the input selector) to a specific input signal,
	// by setting the required value in the CC4yINS.EVxIS register and
	// indicating the active edge of the signal on the CC4yINS.EVxEM
	// register. This event should be then mapped to the capture
	// functionality by setting the CC4yCMC.CAP0S/CC4yCMC.CAP1S with
	// the proper value.
	// Event 0: 3 clock LPF, active high, rising edge, input B (P0.7)
	// Event 1: active high, rising edge, input I (SCU.GSC40)
	CCU4_CC41INS = (1 << 25) | (1 << 18) | (1 << 16) | (8 << 4) | (1);
	CCU4_CC41CMC = (1 << 4) | 2;  // Event 0=capture 0, event 1=start.

	CCU4_CC41SWR = 0x00000f0f;  // Clear interrupt flags.
	CCU4_CC41SRS = (0 << 2);  // Event 0 generates CC40.SR0 (shared).
	CCU4_CC41INTE = (1 << 8);  // Event 0 interrupt enable

	// Enable interrupt node 21-24 at priority 64
	enableInterrupt(21, 64);
	enableInterrupt(22, 64);
	enableInterrupt(23, 64);
	enableInterrupt(24, 64);
}

unsigned int capture_vals[128];
unsigned int capture_head = 0;
void CCU40_SR0(void) {
	// Check CCU4_CC41INTS to determine where the interrupt came from.
	if (CCU4_CC40INTS) {
		// This will occur due to slice 0 activity, no interrupts
		// are actually generated/expected.
		CCU4_CC40SWR = 0x00000f0f;  // Clear interrupt flags.
	}
	if (CCU4_CC41INTS) {
		if (CCU4_CC41INTS & BIT8) {  // Event 0
			// Slice 1 capture event.
			const unsigned int capture_val = CCU4_CC41C1V;
			if (capture_val & BIT20) {
				capture_vals[capture_head] = capture_val;
			} else {
				// Capture full flag not set, value invalid.
				// This was unexpected.
				capture_vals[capture_head] = 0xaa55aa55;
			}
			capture_head = (capture_head + 1) & 0x7F;
		}
		CCU4_CC41SWR = 0x00000f0f;  // Clear interrupt flags.
	}
	if (CCU4_CC42INTS) {
		// This was unexpected.
		CCU4_CC42SWR = 0x00000f0f;  // Clear interrupt flags.
	}
	if (CCU4_CC43INTS) {
		// This was unexpected.
		CCU4_CC43SWR = 0x00000f0f;  // Clear interrupt flags.
	}
}

void CCU40_SR1(void) {
}

void CCU40_SR2(void) {
}

void CCU40_SR3(void) {
}
