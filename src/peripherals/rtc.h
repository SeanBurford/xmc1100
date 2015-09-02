#ifndef PERIPHERALS_RTC_H
#define PERIPHERALS_RTC_H

unsigned int rtcEnable(const unsigned int year,
                       const unsigned int month,
                       const unsigned int day,
                       const unsigned int hour,
                       const unsigned int minute,
                       const unsigned int second);
unsigned int rtcDisable(void);
unsigned int rtcSetDateTime(const unsigned int year,
                            const unsigned int month,
                            const unsigned int day,
                            const unsigned int hour,
                            const unsigned int minute,
                            const unsigned int second);
unsigned int rtcGetDateTime(unsigned int *year,
                            unsigned int *month,
                            unsigned int *day,
                            unsigned int *hour,
                            unsigned int *minute,
                            unsigned int *second);
unsigned int rtcSetPeriodicEvent(const unsigned int mask);
unsigned int rtcClearPeriodicEvent(void);
unsigned int rtcSetAlarm(const unsigned int year,
                         const unsigned int month,
                         const unsigned int day,
                         const unsigned int hour,
                         const unsigned int minute,
                         const unsigned int second,
                         const unsigned int mask);
unsigned int rtcClearAlarm(void);

// Mask values
#define MSKSR_MPSE BIT0
#define MSKSR_MPMI BIT1
#define MSKSR_MPHO BIT2
#define MSKSR_MPDA BIT3
#define MSKSR_MPMO BIT5
#define MSKSR_MPYE BIT6
#define MSKSR_MPALL (MSKSR_MPYE | MSKSR_MPMO | MSKSR_MPDA | MSKSR_MPHO | \
                     MSKSR_MPMI | MSKSR_MPSE)
#define MSKSR_MAI  BIT8

#endif
