#include "xmc1100.h"
#include "gpio.h"
#include "nvic.h"
#include "usic.h"

unsigned int postReset(void)
{
	// 12.4.2 Reset Status
	unsigned int status;
	status = SCU_RSTSTAT & 0x3FF;
	SCU_RSTCLR = BIT0;
	// Reset on ECC error, loss of clock.
	SCU_RSTCON = BIT1 | BIT0;
	return status;
}

void configureClock(void)
{
	SCU_PASSWD = 0xC0;
	// CNTADJ = 1024 clock cycles
	// RTC gets clock from standby source
	// PCLK = 2 * MCLK (64MHz) (fdiv=00/256, idiv=01 (32MHz))
	// MCLK = 32MHz
	SCU_CLKCR = 0x3FF10100;
	// SCU_CLKCR = 0x3ff00400;  // 8MHz
	while (SCU_CLKCR & (BIT30 | BIT31)); // wait for vccp to stabilise.
	SCU_PASSWD = 0x00C3;
}

static volatile unsigned int txcount = 0;

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
	disable_interrupts();
	postReset();
	configureClock();

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
