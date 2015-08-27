#include "peripherals/xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/gpio.h"
#include "peripherals/usic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"
#include "usic_buffer.h"

unsigned int postReset(void)
{
	unsigned int reason = scuResetReason();
	scuResetControl(RSTCON_ALL);
	scuClockControl(CLKCR_M32_P64);
	return reason;
}

int main()
{
	disable_interrupts();
	postReset();

        enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
        enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
        enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	usicEnable();
	usicConfigureCh0();

	ccuEnable();
	// INS Event 1: active high, rising edge, input I (SCU)
	ccuConfigureSlice0(EV1IS_INyI | EV1EM_RISING);
	// INS Event 0: 3 clock LPF, active high, rising edge, input B (P0.7)
	// INS Event 1: active high, rising edge, input I (SCU.GSC40)
	ccuConfigureSlice1(EV0IS_INyB | EV0EM_RISING | LPF0M_3 |
	                   EV1IS_INyI | EV1EM_RISING);
	// CCU40 OUT0 is connected to P0.0, P0.5, P0.6, P1.0, P2.0.
        // Set P0.7 to pull up input (wire up to P0.6 CCU4.OUT0)
	// P0.7 is configured in ccuConfigureSlice1 as the capture trigger.
	enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	enablePin(0, 6, GPIO_OUT_OD_ALT4);  // P0.6 alt4 is CCU4.OUT0
        enablePin(0, 7, GPIO_IN_PU);

	// Clock = 64MHz so 8000000 - 1 is 8 systicks/second.
	systickEnable(8000000 - 1);

	enable_interrupts();

	// Start CCU slices 0 and 1
	ccuStartSlices(BIT1 | BIT0);

	usicBufferSendCh0("Ready.\r\n");
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
