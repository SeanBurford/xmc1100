#include "xmc1100.h"
#include "ccu.h"
#include "nvic.h"
#include "scu.h"

// Start requested slices.
// Uses SCU.GSC40 to start slices simultaneously if CCU4_CC4xINS and
// CCU4_CC4xCMC have been configured to start the slices on an SCU event.
//   slices is a bit field of slices to start (0=slice 0 .. 3=slice3).
void ccuStartSlices(const unsigned int slices) {
	CCU4_GIDLS = BIT9;  // clear the prescaler and load PVAL.
	// BIT8 enables the prescaler run bit for the CCU.
	CCU4_GIDLC = BIT8 | slices;
	// Start CCU40 timers that have been configured (INS and CMC registers)
	// to use SCU.GSC40 as a start event.
	SCU_CCUCON = BIT0;
	// Remove idle mode from the timer slices that were not started above.
	if (slices & BIT0) {
		CCU4_CC40TCSET = BIT0; // set timer slice 0 run bit.
	}
	if (slices & BIT1) {
		CCU4_CC41TCSET = BIT0;
	}
	if (slices & BIT2) {
		CCU4_CC42TCSET = BIT0;
	}
	if (slices & BIT3) {
		CCU4_CC43TCSET = BIT0;
	}
}

// Put requested slices into idle (clock stopped, registers not cleared).
//   slices is a bit field of slices to stop (0=slice 0 .. 3=slice3).
void ccuStopSlices(const unsigned int slices) {
	CCU4_GIDLS = slices;  // Put requested slices into idle.
}

unsigned int ccuEnable(void) {
	// Check module identification
	if ((CCU4_MIDR >> 8) != 0x0000A6C0) {
		return 1;
	}

	// Enable interrupt node 21-24 at priority 64
	enableInterrupt(21, 64);
	enableInterrupt(22, 64);
	enableInterrupt(23, 64);
	enableInterrupt(24, 64);

	// CCU4 clock is SCU fPCLK.
	scuUngatePeripheralClock(CGATCLR0_CCU40);

	// Stop any slices that were previously running.
	ccuStopSlices(BIT3 | BIT2 | BIT1 | BIT0);

	// Deassert synchronised start signal.
	SCU_CCUCON = 0;

	// Configure the global CCU4 register.  17.7.1
	// Request shadow transfer of period and compare registers.
	// Module clock is prescaler clock.
	// Prescaler cleared by software only.
	// Suspend is ignored.
	CCU4_GCTRL = 0;

	return 0;
}

//  input_selector configures input events.  It's a bitwise or of
//    EVyIS | EVyEM | EVyLM | LPFyM for each event y.
//  connections configures the connection matrix. Bitwise or of
//    STRTS | ENDS | CAP0S | CAP1S | CAP2S | GATES | UDS | LDS | CNTS | OFS |
//    TS | MOS | TCE
//  timer_control configures timer behaviour. See 'CCU4yTC timer control'
//    in ccu.h (there are lots of flags here).
void ccuConfigureSlice0(const unsigned int input_selector,
                        const unsigned int connections,
                        const unsigned int timer_control) {
	// Prescaler frequency is Fccu/(PSC^2). 0=/1, 1 = /2, 2=/4, 3=/8 etc.
	CCU4_CC40PSC = 4;  // 64 / 16 = 4MHz
	CCU4_CC40TC = timer_control;
	CCU4_CC40INS = input_selector;
	CCU4_CC41CMC = connections;  // Set after timer_control.
	// Set external output to passive low external high.
	// output = value ^ PSL
	CCU4_CC40PSL = 0;  //Set PSL bit to request passive low.

	// Set the period match and compare level shadow registers.
	// Request transfer of shadow registers to the normal registers.
	// Using PRS blocks out C2V and C3V capture registers.
	// Using CRS blocks out C0V and C1V capture registers.
	CCU4_CC40PRS = 99;  // 40kHz
	CCU4_CC40CRS = 50;
	CCU4_GCSS = BIT0;  // Request transfer of PRS and CRS (slice 0)

	CCU4_CC40INTE = 0;  // No interrupts
}

void ccuConfigureSlice1(const unsigned int input_selector,
                        const unsigned int connections,
                        const unsigned int timer_control) {
	CCU4_CC41PSC = 4;  // 64 / 16 = 4MHz
	CCU4_CC41TC = timer_control;
	CCU4_CC41INS = input_selector;
	CCU4_CC41CMC = connections;  // Set after timer_control.
	CCU4_CC41PSL = 0;

	CCU4_CC41PRS = 0xFFFF;
	CCU4_CC41CRS = 0xFFFF;
	CCU4_GCSS = BIT4;  // Request transfer of PRS and CRS (slice 1)

	CCU4_CC41SWR = 0x00000f0f;  // Clear interrupt flags.
	CCU4_CC41SRS = (0 << 2);  // Event 0 generates CC40.SR0 (shared).
	CCU4_CC41INTE = (1 << 8);  // Event 0 interrupt enable
}

unsigned int capture_vals[128];
unsigned int capture_head = 0;
void __attribute__((interrupt("IRQ"))) CCU40_SR0(void) {
	// Check CCU4_CC41INTS to determine which slice the interrupt came from.
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

void __attribute__((alias("CCU40_SR0"))) CCU40_SR1(void);
void __attribute__((alias("CCU40_SR0"))) CCU40_SR2(void);
void __attribute__((alias("CCU40_SR0"))) CCU40_SR3(void);
