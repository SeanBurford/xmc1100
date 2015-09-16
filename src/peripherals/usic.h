// Support for the XMC1100 USIC (universal serial interface channel)

#ifndef PERIPHERALS_USIC_H
#define PERIPHERALS_USIC_H

unsigned int usicEnable(void);
unsigned int usicConfigureCh0(void);
void usicSendCh0(void);
extern void __attribute__((weak)) usicCh0Receive(unsigned int);
extern unsigned int __attribute__((weak)) usicCh0TransmitDone(void);
extern unsigned char __attribute((weak)) usicCh0Transmit(void);

#endif
