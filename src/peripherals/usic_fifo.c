#include "xmc1100.h"
#include "peripherals/usic.h"

#include "usic_fifo.h"

#define TXBUFF_LEN 128
static char ch0TxBuff[TXBUFF_LEN];
static unsigned int ch0TxBuffStart = 0;
static unsigned int ch0TxBuffEnd = 0;

static const char hex_digits[] = "0123456789abcdef";

void toHex(const unsigned int val, char *buff) {
        unsigned int index = 0;

        for (index = 0; index < 8; index++) {
                unsigned int x = (val >> (4 * (7 - index)));
                buff[index] = hex_digits[(x & 0x0f)];
        }
        buff[index] = '\0';
}

unsigned int usicFifoEnable(void) {
	// Perform usic startup initialization.
	if (usicEnable() != 0) {
		return 1;
	}
	return usicConfigure(0, USIC_PROTO_ASC);
}

void usicFifoSendCh0(const char *msg) {
	unsigned int i, pos;
	pos = ch0TxBuffEnd;
	for(i=0; (msg[i] != '\0') && (pos < TXBUFF_LEN); i++, pos++) {
		ch0TxBuff[pos] = msg[i];
	}
	ch0TxBuffEnd = pos;
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
	if (ch0TxBuffStart >= ch0TxBuffEnd) {
		ch0TxBuffStart = ch0TxBuffEnd = 0;
		return 1;
	}
	return 0;
}

// Output character handler.
unsigned short usicCh0Transmit(void) {
	if (ch0TxBuffStart < ch0TxBuffEnd) {
		return ch0TxBuff[ch0TxBuffStart++];
	} else {
		return '?';
	}
}

