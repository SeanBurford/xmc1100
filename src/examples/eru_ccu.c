// ERU triggering the CCU example.

// Test setup: P0.8 wired to P2.7, P2.10 wired to P2.9.
//
// The ERU configuration is:
// P2.7 -> ERU0.ETL3A1 -> ERU0.OGU0 -&-> CCU40.IN0J (PDOUT) and IN0K (IOUT)
// P2.9 -> ERU0.ETL3B0 -> ERU0.OGU0 /
// We don't use IN0K here.
//
// The CCU configuration as a single shot:
// ERU0.OGU0.PDOUT -> CCU40.IN0J -(rising edge)->  CCU40.EV0 (start count at 0)
// ERU0.OGU0.PDOUT -> CCU40.IN0J -(falling edge)-> CCU40.EV1 (capture)
//
// The GPIO pins were chosen based on this table showing that XMC2Go directly
// exposes input A and B pins for ERU channels 2 and 3 (further pins might be
// exposed through the VADC0 boundary flag):
// ERU Channel 0 B: P2.0  ERU0.0B0
// ERU Channel 2 A: P2.6  ERU0.2A1
// ERU Channel 2 B: P2.10 ERU0.2B0, P2.11 ERU0.2B1
// ERU Channel 3 A: P2.7  ERU0.3A1
// ERU Channel 3 B: P2.9  ERU0.3B0
//
// CCU0.SR[3:0] are connected to corresponding ERU output gates ERU0.OGU[3:0]1.
//
// A trigger output may be selected for each event trigger logic unit.
// Each trigger output routes to the output gating unit with the same number.
// ERU.IOUT[3:0] maps to CCU40.IN[3:0]K plus NVIC.ERU.SR[3:0] respectively.
//
// The pattern detect outputs are available as CCU40 slice inputs and at ports.
// ERU0.PDOUT0: CCU40.IN0J, IN1D, P0.0(exposed), P2.11(exposed)
// ERU0.PDOUT1: CCU40.IN0D, IN1J, P0.1(x), P2.10(exposed)
// ERU0.PDOUT2: CCU40.IN2J, IN3D, P0.2(x), P2.1(x)
// ERU0.PDOUT3: CCU40.IN2D, IN3J, P0.3(x), P2.0(exposed)

#include "xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/eru.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"

struct capture_info {
	unsigned int count;
	unsigned int captures[32];

	unsigned int count_irq;
	unsigned int count_ev0;
	unsigned int count_ev1;
	unsigned int count_period;
} capture_info = {};

unsigned int eru0ConfigureChannel3AorB(void) {
	// Configure ERU ETL3 and OGU0 as P2.7|P2.9.
	eruEnable();

	// The two main ways to trigger an event from OGU are:
	//   EXICON_PE_ENABLE: Trigger event on matching edge (RE, FE).
	//   EXOCON_GEEN_ENABLE: Trigger event on pattern detect change.
	// Whether an event generates an interrupt depends on GP:
	//   EXOCON_GP_GOUT_DISABLE_IOUT_DISABLE: No interrupt.
	//   EXOCON_GP_GOUT_ENABLE_IOUT_TOUT: Interrupt for events.
	//   EXOCON_GP_GOUT_PDOUT_IOUT_TOUT: Interrupt for events if PD.
	//   EXOCON_GP_INV_GOUT_PDOUT_IOUT_TOUT: Interrupt for events if !PD.
	eruConfigureETL3(
	    EXISEL_EXS_IN1,  // ERU.3A1 is P2.7.
	    EXISEL_EXS_IN0,  // ERU.3B0 is P2.9.
	    EXICON_LD_REBUILD |  // FL reflects pattern detect (not sticky).
	    EXICON_PE_DISABLE |  // Don't output an event on rising edge.
	    EXICON_RE_ENABLE |   // Rising edge detect.
	    EXICON_FE_DISABLE |  // Falling edge not detected.
	    EXICON_OCS_OGU0 |    // Trigger output channel OGU0.
	    EXICON_SS_A_AND_B |   // A and B.
	    EXICON_NA_DIRECT |   // A not inverted.
	    EXICON_NB_DIRECT);   // B not inverted.
	eruConfigureOGU0(
	    EXOCON_ISS_DISABLE |  // Peripheral trigger source disabled.
	    EXOCON_GEEN_DISABLE |  // Trigger event on pattern detection change
	    EXOCON_GP_GOUT_DISABLE_IOUT_DISABLE | // Interrupts on events.
	    EXOCON_IPEN_EXICON3);  // Pattern detect on ETL3 output.
	return 0;
}

unsigned int ccuConfigureCaptureERUChannel0(void) {
	// One CCU4 channel serves as both the pulse width and pulse to pulse
	// period timer.
	ccuConfigureSlice(0,
		// CCU40.IN0K is ERU0.IOUT0, CCU40.IN0J is ERU0.PDOUT0.
		ccuEvent0(EVIS_INyJ, EVEM_RISING, EVLM_HIGH, EVLPFM_0) |
		ccuEvent1(EVIS_INyJ, EVEM_FALLING, EVLM_HIGH, EVLPFM_3),
		// Start on event 0, capture on event 1.
		STRTS_EV0 | CAP0S_EV1,
		// Single shot, capture mode, clear on start, capt0->c0v/c1v
		TSSM_ENABLE | CMOD_CAPTURE | STRM_BOTH | SCE_CAPT01,
		PSC_FCCU_8192,  // Prescaler: 8MHz / 8192 = 976.5kHz
		4883, 0,  // 5s period.
		// Interrupt SR0 on EV0 (start), EV1 (capture) and period match
		INTE_E0AE_ENABLE | INTE_E1AE_ENABLE | INTE_PME_ENABLE,
		SRS_E0SR_SR0 | SRS_E1SR_SR0 | SRS_POSR_SR0,
		0);      // Passive level low
	// Enable the slice so that start events can start it.
	ccuEnableSlices(BIT0);
	return 0;
}

void __attribute__((interrupt("IRQ"))) CCU40_SR0(void) {
	// Here SR0 is dedicated to ERU PD related interrupts.
	capture_info.count_irq++;

	if (CCU4_INTS(CC40) & BIT8) {
		// Slice 0 EV0 activity.
		// A match has started (has the lpf period of 3 clocks
		// has passed?)
		// Record the state of P2.7 and P2.9.
		CCU4_SWR(CC40) = BIT8;  // Clear EV0 interrupt flag.
		setPin(1, 1);
		capture_info.count_ev0++;
	}

	if (CCU4_INTS(CC40) & BIT9) {
		// Slice 0 EV1 activity.
		// A match has ended (has the lpf period of 3 clocks passed?)
		// Read the capture register to get the pulse width.
		CCU4_SWR(CC40) = BIT9;  // Clear EV1 interrupt flag.

		unsigned int capture = CCU4_C1V(CC40);
		unsigned int capture_index = (capture_info.count++) & 31;
		capture_info.captures[capture_index] = capture;

		clearPin(1, 1);
		capture_info.count_ev1++;
	}

	if (CCU4_INTS(CC40) & BIT0) {
		// Slice 0 period match activity.
		// Pulse to pulse period has expired.
		CCU4_SWR(CC40) = BIT0;  // Clear period match interrupt flag.

		capture_info.count_period++;
	}
}

unsigned int count = 0;
void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	// P1.0 indicates that systick fired.
	togglePinP1(0);
	count = (count + 1) & 3;
	if (count & 1) {
		setPin(0, 8);
	} else {
		clearPin(0, 8);
	}
	if (count & 2) {
		setPin(2, 10);
	} else {
		clearPin(2, 10);
	}
}

int main()
{
	// Changing this clock speed also requires a change to the argument
	// to systickEnable().
	scuPostReset(CLKCR_M8_P8);

	// Configure GPIO
	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input
	enablePin(2, 7, GPIO_IN_PU);  // P2.7 ERU0 Channel 3 IN A
	enablePin(2, 9, GPIO_IN_PU);  // P2.9 ERU0 Channel 3 IN B
	clearPin(1, 0);
	clearPin(1, 1);

	// Test signal pins.
	enablePin(0, 8, GPIO_OUT_OD);  // P0.8 ALT4  is CCU40.OUT2
	enablePin(2, 10, GPIO_OUT_OD); // P2.10 ALT2 is CCU40.OUT2
	clearPin(0, 8);
	clearPin(2, 10);

	eru0ConfigureChannel3AorB();
	ccuEnable(GCTRL_SUSCFG_ROLLOVER);
	ccuConfigureCaptureERUChannel0();

	// 1Hz (8MHz / 8M)
	systickEnable(8000000 - 1);

	enable_interrupts();

	while(1)
	{
		asm("wfi");
	}
	return 0;
}
