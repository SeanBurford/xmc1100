#include "xmc1100.h"
#include "ccu.h"
#include "nvic.h"
#include "scu.h"

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

void ccuStopSlices(const unsigned int slices) {
	CCU4_GIDLS = slices;  // Put requested slices into idle.
}

unsigned int ccuEnable(const unsigned int global_control) {
	// CCU4 clock is SCU fPCLK.
	scuUngatePeripheralClock(CGATCLR0_CCU40);

	// Check module identification
	// This is not available before peripheral clock starts.
	if ((CCU4_MIDR >> 8) != 0x0000A6C0) {
		return 1;
	}

	// Enable interrupt node 21-24 at priority 64
	enableInterrupt(21, 64);
	enableInterrupt(22, 64);
	enableInterrupt(23, 64);
	enableInterrupt(24, 64);

	// Stop any slices that were previously running.
	ccuStopSlices(BIT3 | BIT2 | BIT1 | BIT0);

	// Deassert synchronised start signal.
	SCU_CCUCON = 0;

	// Configure the global CCU4 register.  17.7.1
	// Request shadow transfer of period and compare registers.
	// Module clock is prescaler clock.
	// Prescaler cleared by software only.
	CCU4_GCTRL = global_control;

	return 0;
}

void ccuSetPeriodCompareSlice0(const unsigned int period,
                               const unsigned int compare) {
	// Set the period match and compare level shadow registers.
	// Request transfer of shadow registers to the normal registers.
	// Using PRS blocks out C2V and C3V capture registers.
	// Using CRS blocks out C0V and C1V capture registers.
	CCU4_CC40PRS = period;
	CCU4_CC40CRS = compare;
	CCU4_GCSS = BIT0;  // Request transfer of PRS and CRS (slice 0)
}

void ccuConfigureSlice0(const unsigned int input_selector,
                        const unsigned int connections,
                        const unsigned int timer_control,
                        const unsigned int prescaler,  // 0x00 - 0x0f
                        const unsigned int period,     // 0x0000 - 0xffff)
                        const unsigned int compare,    // 0x0000 - 0xffff)
                        const unsigned int interrupt_enable,
                        const unsigned int interrupt_route,
                        const unsigned int passive_level) {
	CCU4_CC40PSC = prescaler;
	CCU4_CC40TC = timer_control;
	CCU4_CC40INS = input_selector;
	CCU4_CC40CMC = connections;  // Set after timer_control.
	// Set external output passive level.  output = value ^ PSL
	CCU4_CC40PSL = passive_level;

	ccuSetPeriodCompareSlice0(period, compare);

	CCU4_CC40INTE = interrupt_enable;
	if (interrupt_enable) {
		CCU4_CC40SWR = 0x00000f0f;  // Clear interrupt flags.
		CCU4_CC40SRS = interrupt_route;
	}
}

void ccuConfigureSlice1(const unsigned int input_selector,
                        const unsigned int connections,
                        const unsigned int timer_control,
                        const unsigned int prescaler,
                        const unsigned int period,     // 0x0000 - 0xffff)
                        const unsigned int compare,    // 0x0000 - 0xffff)
                        const unsigned int interrupt_enable,
                        const unsigned int interrupt_route,
                        const unsigned int passive_level) {
	CCU4_CC41PSC = prescaler;
	CCU4_CC41TC = timer_control;
	CCU4_CC41INS = input_selector;
	CCU4_CC41CMC = connections;  // Set after timer_control.
	CCU4_CC41PSL = passive_level;

	CCU4_CC41PRS = period;
	CCU4_CC41CRS = compare;
	CCU4_GCSS = BIT4;  // Request transfer of PRS and CRS (slice 1)

	CCU4_CC41INTE = interrupt_enable;
	if (interrupt_enable) {
		CCU4_CC41SWR = 0x00000f0f;
		CCU4_CC41SRS = interrupt_route;
	}
}
