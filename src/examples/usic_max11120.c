// Read SPI input from an MAX1112x ADC and output it via ASC serial.
//
// Designed for the MAX11125 (12 bits, 8 channels) in a MAXREFDES77, but
// should work with the full MAX1112x range:
//   MAX11120, MAX11121, MAX11122, MAX11123, MAX11124,
//   MAX11125, MAX11126, MAX11127, MAX11128
// https://datasheets.maximintegrated.com/en/ds/MAX11120-MAX11128.pdf
//
// The MAXREFDES77 inputs are:
//   AIN0 V1 ((1/(5.6 / 125.6) * 3.3) / 4096) =~ 0.018v/LSB.
//   AIN1 I1 (voltage across 0.005 ohms amplified by 12.5/20/50/100 times).
//   AIN2 V2 ((1/(5.6 / 125.6) * 3.3) / 4096) =~ 0.018v/LSB.
//   AIN3 I2 (voltage across 0.005 ohms amplified by 12.5/20/50/100 times).
//   AIN4 I2 MAX6607 temperature output.
//
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

unsigned int ch0_cbase = 0;
unsigned int ch1_cbase = 0;

void max112xToASC(const unsigned int adc_cbase) {
	// Send already buffered words from the ADC to serial.
	char buff[32];
	rbufsr = USIC0_RBUFSR(adc_cbase);
	if (rbufsr & (BIT13 | BIT14)) {
		buff[8] = '\r';
		buff[9] = '\n';
		buff[10] = '\0';
		while (rbufsr & (BIT13 | BIT14)) {
			rbufsr = USIC0_RBUFSR(adc_cbase);
			rbuf = USIC0_RBUF(adc_cbase);

			toHex(rbuf, &buff[0]);
			buff[8] = '\r';
			usicBufferedSendCh0(&buff[4]);
		}
		USIC0_PSR(adc_cbase) = 0;
	}
}

void SPISend(const unsigned int cbase, const unsigned short word) {
	USIC0_TBUF(cbase)[0] = query;
	while (USIC0_TCSR(cbase) & 0x80)
		;
}

unsigned int configureMax1112x(const unsigned int adc_cbase) {
	// Config: ADC mode control.
	SPISend(adc_cbase,
	        (0 << 15) |  // REG_CNTL select ADC mode control.
	        (3 << 11) |  // standard_int scan 0-N internal clock w/avg.
	        (5 << 7)  |  // CHSEL scan up to channel 5.
	        (2 << 5)  |  // RESET reset all registers to defaults.
	        (0 << 3)  |  // PM power management normal.
	        (0 << 2)  |  // CHAN_ID set to 1 in ext mode to get chan id.
	        (0 << 1);    // SWCNV set to 1 to convert on rising CS edge.
	max112xToASC(adc_cbase);

	// Config: ADC configuration.
	SPISend(adc_cbase,
	        (0x10 << 11) |  // CONFIG_SETUP
	        (0 << 10)    |  // REFSEL external single ended.
	        (0 << 9)     |  // AVGON averaging turned off.
	        (0 << 7)     |  // NAVG 1 conversion per result.
	        (0 << 5)     |  // NSCAN scans N and returns 4 results.
	        (0 << 3)     |  // SPM all circuitry powered all the time.
	        (0 << 2);       // ECHO disabled.
	max112xToASC(adc_cbase);

	// Defaults are good for:
	// Config: Unipolar.
	// Config: Bipolar.
	// Config: RANGE.
	// Config: Custom scan 0.
	// Config: Custom scan 1.
	// Config: Sample set.

	usicBufferedSendCh0("Configured\r\n");

	return result;
}

unsigned int tickleMax1112x(const unsigned int adc_cbase) {
	SPISend(adc_cbase, 0);
	max112xToASC(adc_cbase);
}

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
        togglePinP1(0);

	for (channel = 0; channel < 10; channel++) {
		tickleMax1112x(ch1_cbase);
	}
	usicBufferedSendCh0("\r\n");
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

	enable_interrupts();

	configureMax1112x(ch1_cbase);

        systickEnable(8000000 - 1);

	usicBufferedSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}
	return 0;
}
