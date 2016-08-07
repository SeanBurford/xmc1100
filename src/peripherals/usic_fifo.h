// Support for the XMC1100 USIC FIFO.

#ifndef PERIPHERALS_USIC_FIFO_H
#define PERIPHERALS_USIC_FIFO_H

unsigned int usicFifoEnable(void);
void usicFifoSendCh0(const char *msg);

void toHex(const unsigned int val, char *buff);

#endif  // PERIPHERALS_USIC_FIFO_H
