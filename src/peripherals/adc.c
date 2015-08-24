#include "xmc1100.h"
#include "adc.h"
#include "gpio.h"
#include "scu.h"

int enableADCPin(const unsigned int port, const unsigned int pin,
                 const unsigned int mode) {
	if (port != 2) {
		// Only port 2 of the XMC1100 has A/D functionality.
		return -1;
	}
	enablePin(port, pin, mode);

	// XXX
	return 0;
}

int disableADCPin(const unsigned int port, const unsigned int pin) {
	if (port != 2) {
		// Only port 2 of the XMC1100 has A/D functionality.
		return 1;
	}
	disablePin(port, pin);

	// XXX
	return 0;
}

void adcCalibrate(int wait) {
	VADC0_GLOBCFG |= BIT31;
	while (wait) {
		unsigned int state = (SHS0_CFG >> 28) & 0x0F;
		switch (state) {
		case 1:	// offset calibration active.
			break;
		case 2:	// gain calibration active.
			break;
		case 3:	// startup calibration active.
			break;
		default:
			wait = 0;
		}
	}
}

int adcEnable(void) {
	// Clear gating of ADC in SCU_CGATCLR0
	scuUngatePeripheralClock(CGATCLR0_VADC);

	// Check the module ID
	if (SHS0_ID  >> 8 != 0x000099c0) {
		return -1;
	}

	// Configure analog settings in SHS.
	// AREF = internal reference, upper supply range (BIT1)
	// ANOFF = convertor controlled by ANONS
	// SCWC = enable AREF/ANOFF write (BIT15)
	SHS0_CFG = BIT15 | BIT1;

	// Wait for convertor to become operable.
	while (SHS0_CFG & BIT14) { }

	// Trigger calibration (60us)
	adcCalibrate(1);

	return 0;
}

int adcDisable(void) {
	// Configure analog settings in SHS.
	// AREF = external reference.
	// ANOFF = convertor off (BIT12)
	// SCWC = enable AREF/ANOFF write (BIT15)
	SHS0_CFG = BIT15 | BIT12;

	// Gate the ADC in SCU_CGATCLR0
	scuGatePeripheralClock(CGATCLR0_VADC);

	return 0;
}
