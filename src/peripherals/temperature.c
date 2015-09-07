// Support code for the temperature sensor.
// All TSE events are handled through interrupt SCU_SR1.

#include "xmc1100.h"
#include "nvic.h"
#include "temperature.h"

// Firmware routines
typedef unsigned long (*_CalcTemperaturePtr)(void);
static _CalcTemperaturePtr *_CalcTemperature = (_CalcTemperaturePtr *)0x0000010c;

unsigned int tseEnable(void) {
	// Enable the temperature sensor.  BIT0 = TSE_EN
	TSE_ANATSECTRL = BIT1;

	// Enable the tse done interrupt.
	SCU_SRMSK |= TSE_DONE;

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
	if (SCU_SRMSK & TSE_DONE) {
		SCU_SRMSK ^= TSE_DONE;
	}

	// Disable the temperature sensor.
	TSE_ANATSECTRL = 0;

	return 0;
}

unsigned long tseRead(void) {

	// Get the latest raw value
	// unsigned short raw_temp = TSE_ANATSEMON;

	// Get calibrated temperature in kelvin.
	return (*_CalcTemperature)();
}

