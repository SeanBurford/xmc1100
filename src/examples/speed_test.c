// Send characters to/from the debug serial port.

#include "xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/usic.h"

static unsigned int txcount = 0;

void usicCh0Receive(unsigned int val) {
	val = val & 0xFF;
	*USIC0_CH0_IN = val;
	if ((unsigned char)val == '\r') {
		*USIC0_CH0_IN = '\n';
	}
	if (val == 'x') {
		txcount = 115200;
		usicSendCh0();
	}
}

unsigned int usicCh0TransmitDone(void) {
	return txcount == 0;
}

unsigned char usicCh0Transmit(void) {
	if (txcount == 0) {
		return 'x';
	}
	txcount -= 1;
	unsigned int index = (txcount & 0x3f);
	if (index == 2) {
		return ((txcount >> 6) & 0x3f) + 0x3e;
	} else if (index == 1) {
		return '\r';
	} else if (index == 0) {
		return '\n';
	}
	return (unsigned char)(index + 0x3e);
}

int main()
{
	scuPostReset(CLKCR_M32_P64);

	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	usicEnable();
	usicConfigureCh0();

	enable_interrupts();

	clearPin(1, 0);
	clearPin(1, 1);

	while(1)
	{
		asm("wfi");
	}
	return 0;
}
