void usicEnable(void);
void usicConfigureCh0(void);
void usicSendCh0(void);
extern void __attribute__((weak)) usicCh0Receive(unsigned int);
extern unsigned int __attribute__((weak)) usicCh0TransmitDone(void);
extern unsigned char __attribute((weak)) usicCh0Transmit(void);
