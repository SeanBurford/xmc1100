// Drive an ultrasonic transducer using the capture compare unit.

#include "xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"

int main()
{
	scuPostReset(CLKCR_M32_P64);

	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	// Capture compare unit config
	ccuEnable(GCTRL_SUSCFG_ROLLOVER);
	// Slice 0: 40kHz 50% PWM.
	// Event 1: active high, rising edge, input I (SCU)
	// Clear the timer (STRM) and start on event 1
	// Transfer shadow registers on timer clear
	ccuConfigureSlice(0,
	    ccuEvent1(EVIS_INyI, EVEM_RISING, EVLM_HIGH, EVLPFM_0),
	    STRTS_EV1,
	    CMOD_COMPARE | CLST_ENABLE | STRM_BOTH,
	    PSC_FCCU_16,  // Prescaler: 64MHz / 16 = 4MHz
	    99, 50,  // 40kHz 50%
	    0, 0,    // No interrupts
	    0);      // Passive level low
	// Slice 1: Capture.
	// Event 0: 3 clock LPF, active high, rising edge, input B (P0.7)
	// Capture on event 0.
	// Event 1: active high, rising edge, input I (SCU.GSC40)
	// Clear the timer (STRM) and start on event 1.
	ccuConfigureSlice(1,
	    ccuEvent0(EVIS_INyB, EVEM_RISING, EVLM_HIGH, EVLPFM_3) |
	    ccuEvent1(EVIS_INyI, EVEM_RISING, EVLM_HIGH, EVLPFM_0),
	    CAP0S_EV0 | STRTS_EV1,
	    CMOD_CAPTURE | STRM_BOTH,
	    PSC_FCCU_16,  // Prescaler: 64MHz / 16 = 4MHz
	    0xffff, 0xffff,
	    INTE_E0AE_ENABLE,
	    SRS_E0SR_SR0,  // Event 0 generates interrupt SR0
	    0);  // Passive level low
	// CCU40 OUT0 is connected to P0.0, P0.5, P0.6, P1.0, P2.0.
	// P0.6: open drain PWM output. (CCU4.OUT0)
	// P1.0: push pull PWM output (LED). (CCU4.OUT0)
	// P0.7 pull up input to CCU slice 1 trigger capture.
	enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	enablePin(0, 6, GPIO_OUT_PP_ALT4);  // P0.6 alt4 is CCU4.OUT0
	enablePin(0, 7, GPIO_IN_PU);

	// Clock = 64MHz so 8000000 - 1 is 8 systicks/second.
	systickEnable(8000000 - 1);

	enable_interrupts();

	// Start CCU slices 0 and 1
	ccuStartSlices(BIT1 | BIT0);

	while(1)
	{
		asm("wfi");
	}

	return 0;
}

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	// Toggle LED P1.1.
	togglePinP1(1);
}

unsigned int capture_vals[128];
unsigned int capture_head = 0;
void __attribute__((interrupt("IRQ"))) CCU40_SR0(void) {
	// Check CCU4_INTS to determine which slice the interrupt came from.
	if (CCU4_INTS(CC40)) {
		// This will occur due to slice 0 activity, no interrupts
		// are actually generated/expected.
		CCU4_SWR(CC40) = 0x00000f0f;  // Clear interrupt flags.
	}
	if (CCU4_INTS(CC41)) {
		if (CCU4_INTS(CC41) & BIT8) {  // Event 0
			// Slice 1 capture event.
			const unsigned int capture_val = CCU4_C1V(CC41);
			if (capture_val & BIT20) {
				capture_vals[capture_head] = capture_val;
			} else {
				// Capture full flag not set, value invalid.
				// This was unexpected.
				capture_vals[capture_head] = 0xaa55aa55;
			}
			capture_head = (capture_head + 1) & 0x7F;
		}
		CCU4_SWR(CC41) = 0x00000f0f;  // Clear interrupt flags.
	}
	if (CCU4_INTS(CC42)) {
		// This was unexpected.
		CCU4_SWR(CC42) = 0x00000f0f;  // Clear interrupt flags.
	}
	if (CCU4_INTS(CC43)) {
		// This was unexpected.
		CCU4_SWR(CC43) = 0x00000f0f;  // Clear interrupt flags.
	}
}

