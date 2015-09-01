#include "xmc1100.h"
#include "rtc.h"
#include "scu.h"

// Wait for the SCU to complete existing serial transfers with the RTC.
#define WAIT_FOR_SERIAL while (SCU_MIRRSTS);
#define TIME0(day, hour, minute, second) \
	((day << 24) | (hour << 16) | (minute << 8) | second)
#define TIME1(year, month) ((year << 16) | (month << 8))

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
	WAIT_FOR_SERIAL;
	RTC_CTR = (0x7fff << 16) | BIT0;

	return 0;
}

unsigned int rtcDisable(void) {
	WAIT_FOR_SERIAL;
	RTC_CTR = (0x7fff << 16);
	return 0;
}

unsigned int rtcSetDateTime(const unsigned int year,
                            const unsigned int month,
                            const unsigned int day,
                            const unsigned int hour,
                            const unsigned int minute,
                            const unsigned int second) {
	// Program TIM0 then TIM1
	WAIT_FOR_SERIAL;
	RTC_TIM0 = TIME0(day, hour, minute, second);
	RTC_TIM1 = TIME1(year, month);
	return 0;
}

unsigned int rtcGetDateTime(unsigned int *year,
                            unsigned int *month,
                            unsigned int *day,
                            unsigned int *hour,
                            unsigned int *minute,
                            unsigned int *second) {
	// TIM0 has to be read before TIM1
	WAIT_FOR_SERIAL;
	unsigned int tim0 = RTC_TIM0;
	unsigned int tim1 = RTC_TIM1;
	*year = tim1 >> 16;
	*month = (tim1 >> 8) & 0x0f;
	*day = (tim0 >> 24) & 0x1f;
	*hour = (tim0 >> 16) & 0x1f;
	*minute = (tim0 >> 8) & 0x3f;
	*second = tim0 & 0x3f;
	return 0;
}

unsigned int rtcSetPeriodicEvent(const unsigned int mask) {
	RTC_MSKSR = mask;
	return 0;
}

unsigned int rtcClearPeriodicEvent(void) {
	// Mask the periodic event, keep alarms.
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
	// Program ATIM0 and ATIM1
	WAIT_FOR_SERIAL;
	RTC_ATIM0 = TIME0(day, hour, minute, second);
	RTC_ATIM1 = TIME1(year, month);
	RTC_MSKSR |= MSKSR_MAI;  // Unmask the alarm
	return 0;
}

unsigned int rtcClearAlarm(void) {
	// Mask the alarm, keep periodic service requests.
	RTC_MSKSR &= MSKSR_MPALL;
	return 0;
}

void __attribute__((interrupt("IRQ"))) SCU_SR1(void) {
	unsigned int stssr = RTC_STSSR;
	if (stssr & MSKSR_MPALL) {
		// Periodic service request
	}
	if (stssr & MSKSR_MAI) {
		// Alarm
	}
	// Clear event bits.
	RTC_CLRSR = MSKSR_MAI | MSKSR_MPALL;
}
