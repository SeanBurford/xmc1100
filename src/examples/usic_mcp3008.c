// Read SPI input from an MCP3008 ADC and output it via ASC serial.

// Configure USIC channel 1 as an SSC master using pins:
//   DX0 input: P0.6 (USIC0_CH1.DX0C)
//   DOUT0 output: P0.7 (P0.7 ALT7)
//   SCLKOUT output: P0.8 (P0.8 ALT7)
//   CS SELO0 output: P0.9 (P0.9 ALT7)


#include "xmc1100.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"
#include "peripherals/usic.h"
#include "peripherals/usic_fifo.h"

unsigned int ch0_cbase = 0;
unsigned int ch1_cbase = 0;

unsigned int readMCP3008Channel(int channel) {
	unsigned int result = 0;
	unsigned int rbufsr = 0;
	unsigned int rbuf = 0;

	// MCP3008 expects 1 start bit, sgl/diff and then a 3 bit channel.
	// In return it sends a 10 bit MSB conversion followed by a 10-1 bit LSB
	// conversion.  These start at bit 8 of the received value, so we should
	// lose one bit with a 16 bit read.  A 32 bit read gets all bits.
	const unsigned short query = (0x18 | channel) << 11;
	USIC0_TBUF(USIC0_CH1_BASE)[31] = query;
	while (USIC0_TCSR(USIC0_CH1_BASE) & 0x80)
		;
	// clock in more data
	USIC0_TBUF(USIC0_CH1_BASE)[31] = (unsigned short) 0x0000;
	while (USIC0_TCSR(USIC0_CH1_BASE) & 0x80)
		;

	rbufsr = USIC0_RBUFSR(USIC0_CH1_BASE);
	if (rbufsr & (BIT13 | BIT14)) {
		while (rbufsr & (BIT13 | BIT14)) {
			// unsigned int psr = USIC0_PSR(USIC0_CH1_BASE);
			USIC0_PSR(USIC0_CH1_BASE) = 0;
			rbuf = USIC0_RBUF(USIC0_CH1_BASE);
			result = (result << 16) | rbuf;

			rbufsr = USIC0_RBUFSR(USIC0_CH1_BASE);
		}
	}
	return result;
}

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	char buff[32];
	unsigned int channel, result;

        togglePinP1(0);

	buff[17] = '\r';
	buff[18] = '\n';
	buff[19] = '\0';
	for (channel = 0; channel < 8; channel++) {
		result = readMCP3008Channel(0);
		toHex(channel, &buff[0]);
		buff[8] = ' ';
		toHex(result, &buff[9]);
		buff[17] = '\r';
		usicBufferedSendCh0(buff);
	}

	usicBufferedSendCh0(&buff[17]);
}

int main()
{
	scuPostReset(CLKCR_M32_P64);

	// On board LEDs.
	enablePin(1, 0, GPIO_OUT_PP);
	enablePin(1, 1, GPIO_OUT_PP);

	// Pins used for USIC channel 0 ASC mode.
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);     // debug serial input
	clearPin(1, 0);
	clearPin(1, 1);

	// Pins used for USIC channel 1 SSC mode.
	enablePin(0, 6, GPIO_IN_FLOAT);  // DX0C in
	enablePin(0, 7, GPIO_OUT_PP_ALT7);  // data out
	enablePin(0, 8, GPIO_OUT_PP_ALT7);  // sclk out
	enablePin(0, 9, GPIO_OUT_PP_ALT7);  // chip select out

	ch0_cbase = usicConfigure(0, USIC_PROTO_ASC);
	ch1_cbase = usicConfigure(1, USIC_PROTO_SSC);
	while (ch0_cbase == 0 || ch1_cbase == 0) {
		asm("wfi");
	}
	usicFifoEnable(0);

        systickEnable(8000000 - 1);

	enable_interrupts();

	usicBufferedSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}
	return 0;
}
