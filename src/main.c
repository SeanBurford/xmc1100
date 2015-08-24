#include "xmc1100.h"
#include "ccu.h"
#include "gpio.h"
#include "usic.h"
#include "usic_buffer.h"
#include "scu.h"
#include "systick.h"

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
	ccuConfigureSlice0();
	ccuConfigureSlice1();
	// CCU40 OUT0 is connected to P0.0, P0.5, P0.6, P1.0, P2.0.
        // Set P0.7 to pull up input (wire up to P0.6 CCU4.OUT0)
	// P0.7 is configured in ccuConfigureSlice1 as the capture trigger.
	enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	enablePin(0, 6, GPIO_OUT_OD_ALT4);  // P0.6 alt4 is CCU4.OUT0
        enablePin(0, 7, GPIO_IN_PU);

	systickEnable();

	enable_interrupts();

	ccuStartSlices();

	usicBufferSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}

	return 0;
}
