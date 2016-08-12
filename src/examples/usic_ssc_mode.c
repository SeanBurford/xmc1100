// Read SPI input from an MCP8003 ADC and output it via ASC serial.

// Configure USIC channel 1 as an SSC master using pins:
//   DX0 input: P0.6 (USIC_CH1.DX0C)
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

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	unsigned int rbufsr = 0;
	unsigned int rbuf = 0;
	char buff[64];

        togglePinP1(0);

	// MCP3008 expects 1 start bit, sgl/diff and then a 3 bit channel.
	// In return it sends a 10 bit MSB conversion followed by a 10-1 bit LSB
	// conversion.  These start at bit 8 of the received value, so we should
	// lose one bit with a 16 bit read.  A 32 bit read gets all bits.
	USIC0_CH1_TBUF[0] = (unsigned short) 0xC000;  // 1 (start) 1 (single) 000 (ch0)
	while (USIC0_CH1_TCSR & 0x80)
		;
	USIC0_CH1_TBUF[0] = (unsigned short) 0x0000;  // clock in more data
	while (USIC0_CH1_TCSR & 0x80)
		;

	rbufsr = USIC0_CH1_RBUFSR;
	buff[17] = '\r';
	buff[18] = '\n';
	buff[19] = '\0';
	if (rbufsr & (BIT13 | BIT14)) {
		while (rbufsr & (BIT13 | BIT14)) {
			// unsigned int psr = USIC0_CH1_PSR;
			USIC0_CH1_PSR = 0;
			rbuf = USIC0_CH1_RBUF;
			toHex(rbufsr, &buff[0]);
			buff[8] = ' ';
			toHex(rbuf, &buff[9]);
			buff[17] = '\r';
			buff[18] = '\n';
			buff[19] = '\0';
			usicBufferedSendCh0(buff);
			rbufsr = USIC0_CH1_RBUFSR;
		}
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

	while (usicConfigure(0, USIC_PROTO_ASC) ||
               usicFifoEnable(0) ||
	       usicConfigure(1, USIC_PROTO_SSC)) {
		asm("wfi");
	}

        systickEnable(8000000 - 1);

	enable_interrupts();

	usicBufferedSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}
	return 0;
}
