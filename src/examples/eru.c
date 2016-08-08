// ERU example.

// XMC2Go directly exposes input A and B pins for ERU channels 2 and 3
// (further pins might be exposed through the VADC0 boundary flag):
// Channel 0 B: P2.0  ERU0.0B0
// Channel 2 A: P2.6  ERU0.2A1
// Channel 2 B: P2.10 ERU0.2B0, P2.11 ERU0.2B1
// Channel 3 A: P2.7  ERU0.3A1
// Channel 3 B: P2.9  ERU0.3B0
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
#include "peripherals/eru.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"

void __attribute__((interrupt("IRQ"))) ERU_SR0(void) {
	// Set P1.1 to match OGU0 pattern detect.
	// This is only really valuable if you set EXOCON_GEEN_ENABLE
	// so that events are generated for both PD match and mismatch.
	// You'll want EXOCON_GP_GOUT_ENABLE_IOUT_TOUT because gating on
	// PD will result the LED never changing (after the first change).
	unsigned int exocon0 = EXOCON0;
	if (exocon0 & BIT3) {
		setPin(1, 1);
	} else {
		clearPin(1, 1);
	}
}

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
	    EXICON_PE_ENABLE |  // Output a pulse on select edge detected.
	    EXICON_LD_REBUILD |  // FL reflects pattern detect (not sticky).
	    EXICON_RE_ENABLE |   // Rising edge detect.
	    EXICON_FE_DISABLE |  // Falling edge not detected.
	    EXICON_OCS_OGU0 |    // Trigger output channel OGU0.
	    EXICON_SS_A_AND_B |   // A and B.
	    EXICON_NA_DIRECT |   // A not inverted.
	    EXICON_NB_DIRECT);   // B not inverted.
	eruConfigureOGU0(
	    EXOCON_ISS_DISABLE |  // Peripheral trigger source disabled.
	    EXOCON_GEEN_ENABLE |  // Trigger event on pattern detection change
	    EXOCON_GP_GOUT_ENABLE_IOUT_TOUT | // GOUT enabled, IOUT=TOUT.
	    EXOCON_IPEN_EXICON3);  // Pattern detect on ETL3 output.
	return 0;
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
	enablePin(0, 8, GPIO_OUT_OD);
	enablePin(2, 10, GPIO_OUT_OD);
	clearPin(0, 8);
	clearPin(2, 10);

	eru0ConfigureChannel3AorB();

	// 1Hz (8MHz / 8M)
	systickEnable(8000000 - 1);

	enable_interrupts();

	while(1)
	{
		asm("wfi");
	}
	return 0;
}
