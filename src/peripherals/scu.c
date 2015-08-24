#include "xmc1100.h"

// Bit protection scheme enable and disable.
#define PASSWD_MODE_DISABLED 0x00C0
#define PASSWD_MODE_ENABLED 0x00C3

// Clock control register.
#define CLKCR_VDDC2HIGH BIT31
#define CLKCR_VDDC2LOW BIT30

// Get reason for previous reset (12.4.2)
unsigned int scuResetReason(void)
{
        unsigned int reason;
        reason = SCU_RSTSTAT & 0x3FF;
        SCU_RSTCLR = BIT0;
        // Reset on ECC error, loss of clock.
        SCU_RSTCON = BIT1 | BIT0;
        return reason;
}

// Set reset triggers.
void scuResetControl(const unsigned int rstcon) {
	SCU_RSTCON = rstcon;
}

// Set CPU and peripheral clock speeds.
void scuClockControl(const unsigned int clkcr) {
	SCU_PASSWD = PASSWD_MODE_DISABLED;
	SCU_CLKCR = clkcr;
	SCU_PASSWD = PASSWD_MODE_ENABLED;
	// wait for vccp to stabilise.
	while (SCU_CLKCR & (CLKCR_VDDC2HIGH | CLKCR_VDDC2LOW));
}

// Enable specified peripheral clock.
void scuUngatePeripheralClock(const unsigned int peripheral) {
	SCU_PASSWD = PASSWD_MODE_DISABLED;
	SCU_CGATCLR0 = peripheral;
	SCU_PASSWD = PASSWD_MODE_ENABLED;
	// wait for vccp to stabilise.
	while (SCU_CLKCR & (CLKCR_VDDC2HIGH | CLKCR_VDDC2LOW));
}

// Disable specified peripheral clock.
void scuGatePeripheralClock(const unsigned int peripheral) {
	SCU_PASSWD = PASSWD_MODE_DISABLED;
	SCU_CGATSET0 = peripheral;
	SCU_PASSWD = PASSWD_MODE_ENABLED;
	// wait for vccp to stabilise.
	while (SCU_CLKCR & (CLKCR_VDDC2HIGH | CLKCR_VDDC2LOW));
}
