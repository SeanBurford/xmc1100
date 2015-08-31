#include "xmc1100.h"
#include "rtc.h"

// STSSR is the service request status register... XXX

// Wait for the SCU to complete existing serial transfers with the RTC.
static void rtcCompleteSerialTransfer(void) {
	// XXX SCU_MIRRSTS XXX
}

unsigned int rtcEnable(const unsigned int year,
                       const unsigned int month,
                       const unsigned int day,
                       const unsigned int hour,
                       const unsigned int minute,
                       const unsigned int second) {
	// Ungate the RTC clock.
	scuUngatePeripheralClock(CGATCLR0_RTC);

	// Check module identification
	if ((RTC_ID >> 8) != 0x0000A3C0) {
		return 1;
	}

	// Set the start time.
	rtcSetDateTime(year, month, day, hour, minute, second);

	// RTC is in reset after power up until reset is released.
	// 0x7fff:  prescaler (32768Hz clock/0x7fff = 1 update/second).
	// SUS BIT1: 0: Module is not suspended during debug
	// ENB BIT0: 1: Enable module
	rtcCompleteSerialTransfer()
	RTC_CTR = (0x7fff << 16) | BIT0;

	return 0;
}

unsigned int rtcDisable(void) {
	rtcCompleteSerialTransfer()
	RTC_CTR = (0x7fff << 16);
	return 0;
}

unsigned int rtcSetDateTime(const unsigned int year,
                            const unsigned int month,
                            const unsigned int day,
                            const unsigned int hour,
                            const unsigned int minute,
                            const unsigned int second) {
	rtcCompleteSerialTransfer()

	// Program TIM0 then TIM1


	return 0;
}

unsigned int rtcGetDateTime(unsigned int *year,
                            unsigned int *month,
                            unsigned int *day,
                            unsigned int *hour,
                            unsigned int *minute,
                            unsigned int *second) {
	// TIM0 has to be read before TIM1

	return 0;
}

unsigned int rtcSetPeriodicEvent(const unsigned int mask) {
	RTC_MSKSR = mask;
	return 0;
}

unsigned int rtcClearPeriodicEvent(void) {
	// Mask the periodic event
	RTC_MSKSR &= MSKSR_MAI;
	return 0;
}

unsigned int rtcSetAlarm(const unsigned int year,
                         const unsigned int month,
                         const unsigned int day,
                         const unsigned int hour,
                         const unsigned int minute,
                         const unsigned int second,
                         const unsigned int mask) {
	rtcCompleteSerialTransfer();

	// Program ATIM0 and ATIM1

	// Unmask the alarm
	RTC_MSKSR |= MSKSR_MAI;

	return 0;
}

unsigned int rtcClearAlarm(void) {
	// Mask the alarm
	RTC_MSKSR &= (MSKSR_MPSE | MSKSR_MPMI | MSKSR_MPHO | MSKSR_MPDA |
	              MSKSR_MPMO | MSKSR_MPYE);

	return 0;
}
