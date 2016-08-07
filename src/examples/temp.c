// Make LED P1.0 brighter as the CPU temperature rises.

#include "xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"
#include "peripherals/temperature.h"
#include "peripherals/usic_fifo.h"

int main()
{
	scuPostReset(CLKCR_M32_P64);

	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	usicFifoEnable();

	// Enable the temperature sensor.
	tseEnable();

	// PWM LED P1.0 using CCU slice 0.
	ccuEnable(GCTRL_SUSCFG_PASSIVE);
	ccuConfigureSlice0(
	    // Reset count and start on event 0 (Input I: SCU CCU start event)
	    ccuEvent0(EVIS_INyI, EVEM_RISING, EVLM_HIGH, EVLPFM_0),
	    STRTS_EV0,
	    CMOD_COMPARE | CLST_ENABLE | STRM_BOTH,
	    PSC_FCCU_512,  // Prescaler 8MHz / 512 = 15,625Hz
	    100, 50,       // 50%
	    0, 0, 0);      // Generate no interrupts. passive level low.
	enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	ccuStartSlices(BIT0);

	enable_interrupts();

	usicFifoSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}

	return 0;
}

void __attribute__((interrupt("IRQ"))) SCU_SR1(void) {
	static unsigned int min, max;
	if (SCU_SRRAW & SRMSK_TSE_DONE) {
		const unsigned int temperature = tseRead();
		if ((min == 0) || (temperature < min)) {
			min = temperature;
		}
		if (temperature > max) {
			max = temperature;
		}
		if (min < max) {
			const unsigned int range = max - min;
			const unsigned int ratio = range - (temperature - min);
			ccuSetPeriodCompareSlice0(range, ratio);
		}

		// Clear the event bit in SCU_SRRAW
		SCU_SRCLR = SRMSK_TSE_DONE;
	}
}

// Input character handler.  Echo received characters to output.
void usicCh0Receive(unsigned int val) {
	val = val & 0xFF;
	*USIC0_CH0_IN = val;
	if ((unsigned char)val == '\r') {
		*USIC0_CH0_IN = '\n';
	}
}

