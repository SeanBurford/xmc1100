#include "peripherals/xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/usic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"
#include "peripherals/temperature.h"
#include "usic_buffer.h"

int main()
{
	scuPostReset(CLKCR_M32_P64);

        enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
        enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
        enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	usicEnable();
	usicConfigureCh0();

	tseEnable();

	// Clock = 64MHz so 8000000 - 1 is 8 systicks/second.
	systickEnable(8000000 - 1);

	enable_interrupts();

	usicBufferSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}

	return 0;
}

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
        // Toggle LED P1.1.
	static unsigned long temperature = 0;
	temperature = tseRead();
	temperature += 1;
        togglePinP1(1);
}

void __attribute__((interrupt("IRQ"))) SCU_SR1(void) {
	if (SCU_SRRAW & TSE_DONE) {
		unsigned long temperature = tseRead();

		temperature = temperature + 1;

		// Clear the event bit in SCU_SRRAW
		SCU_SRCLR = TSE_DONE;
	}
}

