// Read SPI input from an MAX1112x ADC and output it via ASC serial.
//
// Designed for the MAX11125 (12 bits, 8 channels) in a MAXREFDES77, but
// should work with the full MAX1112x range:
//   MAX11120, MAX11121, MAX11122, MAX11123, MAX11124,
//   MAX11125, MAX11126, MAX11127, MAX11128
// https://datasheets.maximintegrated.com/en/ds/MAX11120-MAX11128.pdf
//
// The MAXREFDES77 inputs are:
//   AIN0 V1 voltage = ((value / 4096) * 3.3v) * (1 / (5.6 / 125.6))
//                     Voltage is output from a resistor divider.
//                     ~ 0.018V per LSB
//   AIN1 I1 current = (((value / 4096) * 3.3v) / 20) / 0.005 ohms
//                     MAX44285T has an amplification factor of 20.
//                     ~ 0.0080A per LSB
//   AIN2 V2 voltage = ((value / 4096) * 3.3v) * (1 / (5.6 / 125.6))
//   AIN3 I2 current = (((value / 4096) * 3.3v) / 20) / 0.005 ohms
//   AIN4 I2 MAX6607 temperature output.
//           degrees Celsius = (((value / 4096) * 3.3v) - 0.5v) / 0.01v/C
//
// Configure USIC channel 1 as an SSC master using pins:
//   DX0 input: P0.6 (USIC0_CH1.DX0C)
//   DOUT0 output: P0.7 (P0.7 ALT7)
//   SCLKOUT output: P0.8 (P0.8 ALT7)
//   CS SELO0 output: P0.9 (P0.9 ALT7)
// These pins drive the sampling:
//   CNVST output: P2.0
//   EOC end of conversion input: P2.6


#include "xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/eru.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/usic.h"
#include "peripherals/usic_fifo.h"

unsigned int ch0_cbase = 0;
unsigned int ch1_cbase = 0;

void SPISend(const unsigned int cbase, const unsigned short word) {
	// USIC0_TBUF[15] to set frame length to 16 bits.
	USIC0_TBUF(cbase)[15] = word;
	while (USIC0_TCSR(cbase) & 0x80)
		;
}

void tickleMax1112x(void) {
	// In internally clocked modes the MAX112x outputs the oldest word
	// from the FIFO until the FIFO is empty, then it outputs zero.
	// It will perform another round of sampling on the rising edge of
	// CS if SWCNV is set in the ADC Mode Control Register (SWCNV is
	// then cleared after each round of sampling) or it the CNVST pin
	// is pulled low.
	unsigned int buff_index=0;
	char buff[64];
	unsigned int i;
	for (i = 0; i < 8; i++) {
		SPISend(USIC0_CH1_BASE, 0);  // Clock in a word
		unsigned int rbufsr = USIC0_RBUFSR(USIC0_CH1_BASE);
		while (rbufsr & (BIT13 | BIT14)) {
			unsigned int result = USIC0_RBUF(USIC0_CH1_BASE);
			toHexShort(result, &buff[buff_index]);
			buff_index += 4;
			buff[buff_index] = ' ';
			buff_index += 1;
			rbufsr = USIC0_RBUFSR(USIC0_CH1_BASE);
		}
		USIC0_PSR(USIC0_CH1_BASE) = 0;
	}
	buff[buff_index++] = '\r';
	buff[buff_index++] = '\n';
	buff[buff_index++] = '\0';
	usicBufferedSendCh0(buff);
}

void configureMax1112x(const unsigned int adc_cbase) {
	// Config: Reset.
	SPISend(adc_cbase, (2 << 5)); // RESET reset all registers to defaults.

	// Config: ADC configuration.
	SPISend(adc_cbase,
	        (0x10 << 11) |  // CONFIG_SETUP
	        (1 << 10)    |  // REFSEL external single ended (vs REF-).
	        (0 << 9)     |  // AVGON averaging turned off.
	        (0 << 7)     |  // NAVG 1 conversion per result.
	        (0 << 5)     |  // NSCAN scans N and returns 4 results.
	        (0 << 3)     |  // SPM all circuitry powered all the time.
	        (0 << 2));      // ECHO disabled.

	// Config: Unipolar.
	SPISend(adc_cbase, (0x12 << 11));  // All channels unipolar.
	SPISend(adc_cbase, (0x11 << 11) |  // All channels unipolar.
                           (1 << 2));      // Reference all channels to REF-.

	// Config: ADC mode control.
	SPISend(adc_cbase,
	        (0 << 15) |  // REG_CNTL select ADC mode control.
	        (3 << 11) |  // standard_int scan 0-N internal clock w/avg.
	        (4 << 7)  |  // CHSEL scan up to channel 4.
	        (0 << 5)  |  // RESET the FIFO only.
	        (0 << 3)  |  // PM power management normal.
	        (1 << 2));   // CHAN_ID set to 1 in ext mode to get chan id.

	// Defaults are good for:
	// Config: Bipolar.
	// Config: RANGE.
	// Config: Custom scan 0.
	// Config: Custom scan 1.
	// Config: Sample set.

	// Flush anything buffered.
	tickleMax1112x();

	usicBufferedSendCh0("Configured\r\n");
}

void configureCCU(void) {
	// Configure the CCU to send two CNVST pulses a second to P2.0.
	ccuEnable(GCTRL_SUSCFG_ROLLOVER);
	ccuConfigureSlice(
		0,
		ccuEvent0(EVIS_INyI, EVEM_RISING, EVLM_HIGH, EVLPFM_0),
		STRTS_EV0,
		CMOD_COMPARE | CLST_ENABLE | STRM_BOTH,
		PSC_FCCU_1024,  // Prescaler 64MHz / 1024 = 62,500Hz
		31249, 31249,   // 2 Hz with 15uS of trigger time
		0, 0, 1);       // No interrupts, passive level high
	ccuStartSlices(BIT0);
}

void configureERU(void) {
	// Configure the ERU to interrupt on P2.6 EOC going low (ERU0.2A1)
	eruEnable();
	eruConfigureETL2(
		EXISEL_EXS_IN1,  // ERU.2A1 is P2.6
		EXISEL_EXS_IN2,  // ERU.2B2 is reserved
		EXICON_PE_ENABLE |  // Output a pulse on select edge detected.
		EXICON_LD_REBUILD | // Level detect reflects actual level.
		EXICON_RE_DISABLE | // Rising edge ignored.
		EXICON_FE_ENABLE |  // Falling edge trigger.
		EXICON_OCS_OGU0 |   // Trigger output channel OGU0.
		EXICON_SS_A |       // Only input A contributes.
		EXICON_NA_DIRECT |  // A not inverted.
		EXICON_NB_DIRECT);  // B not inverted.
	eruConfigureOGU0(
		EXOCON_ISS_DISABLE |  // Peripheral trigger source disabled.
		EXOCON_GEEN_DISABLE |  // Event on pattern detection change
		EXOCON_GP_GOUT_ENABLE_IOUT_TOUT | // GOUT enabled, IOUT=TOUT.
		EXOCON_IPEN_EXICON2);  // Pattern detect on ETL2 output.
}

void __attribute__((interrupt("IRQ"))) ERU_SR0(void) {
	// ERU OGU0 pattern match event.
	// We're here because EOC was pulled low by the ADC.
	// Clock out the results and send them to the ASC port.
	togglePinP1(0);
	tickleMax1112x();
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

	// CNVST is P2.0
	enablePin(2, 0, GPIO_OUT_PP_ALT2);
	setPin(2, 0);

	configureMax1112x(ch1_cbase);

	// Configure the CCU to send CNVST pulses to P2.0
	configureCCU();
	// Configure the ERU to interrupt on EOC on P2.6
	enablePin(2, 6, GPIO_IN_FLOAT);  // EOC in through ERU ch2.
	configureERU();

	usicBufferedSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}
	return 0;
}
