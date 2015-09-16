// Support code for the temperature sensor.
// All TSE events are handled through interrupt SCU_SR1.

#include "xmc1100.h"
#include "nvic.h"
#include "scu.h"
#include "temperature.h"

// Firmware routine is busted in two ways:
//   One of the constants that is used to multiply the raw value is zero.
//   The code if (val < min) val = min; is actually if (val > min) val = min.
// typedef unsigned int (*_CalcTempPtr)(void);
// static _CalcTempPtr *_CalcTemperature = (_CalcTempPtr *)0x000010c;

unsigned int tseEnable(void) {
	// Enable the temperature sensor.  BIT0 = TSE_EN
	TSE_ANATSECTRL = BIT0;

	// Enable the tse done interrupt.
	SCU_SRMSK |= SRMSK_TSE_DONE;

	// TODO: Enable the tse temperature high/low interrupts.
	// TSE_ANATSEIH.TSE_IH and TSE_ANATSEIH.TSE_IL configure the limits.
	// _CalcTSEVAR calculates the values to use here.
	// SRRAW.TSE_HIGH and SRRAW.TSE_LOW indicate high/low temperature
	// events.

	// Enable the shared SCU SR1 interrupt.
	enableInterrupt(1, 65);

	return 0;
}

unsigned int tseDisable(void) {
	// Disable the tse interrupt.
	if (SCU_SRMSK & SRMSK_TSE_DONE) {
		SCU_SRMSK ^= SRMSK_TSE_DONE;
	}
	// Disable the temperature sensor.
	TSE_ANATSECTRL = 0;
	return 0;
}

unsigned int tseRead(void) {
#ifdef ERRATA_SCU_CM_014
	// Get the latest raw value
	unsigned int temperature = TSE_ANATSEMON;
#else
	// Get calibrated temperature in kelvin.
	unsigned int temperature = (*_CalcTemperature)();
#endif

	return temperature;
}

