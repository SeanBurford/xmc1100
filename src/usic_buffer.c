#include "peripherals/xmc1100.h"
#include "peripherals/usic.h"

static char ch0TxBuff[128];
static unsigned int ch0TxBuffStart = 0;
static unsigned int ch0TxBuffEnd = 0;

void usicBufferSendCh0(const char *msg) {
        unsigned int i;
        for(i=0; msg[i] != '\0'; i++) {
                ch0TxBuff[i] = msg[i];
        }
        ch0TxBuffEnd = i;
        ch0TxBuffStart = 0;
        usicSendCh0();
}

// Input character handler.  Echo received characters to output.
void usicCh0Receive(unsigned int val) {
        val = val & 0xFF;
        *USIC0_CH0_IN = val;
        if ((unsigned char)val == '\r') {
                *USIC0_CH0_IN = '\n';
        }
}

// Output done check.
unsigned int usicCh0TransmitDone(void) {
        if (ch0TxBuffStart < ch0TxBuffEnd) {
		return 0;
	}
	return 1;
}

// Output character handler.
unsigned char usicCh0Transmit(void) {
        return ch0TxBuff[ch0TxBuffStart++];
}

