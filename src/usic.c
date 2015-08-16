#include "xmc1100.h"
#include "nvic.h"
#include "usic.h"

void usicEnable(void) {
	// Clear gating of USIC in SCU_CGATCLR0
	SCU_PASSWD = 0xC0;
	SCU_CGATCLR0 = BIT3;
	SCU_PASSWD = 0xC3;
        while (SCU_CLKCR & (BIT30 | BIT31)); // wait for vccp to stabilise.
}

void usicConfigureCh0(void) {
	// Enable USIC module clock and functionality.
	USIC0_CH0_KSCFG = BIT1 | BIT0;
	USIC0_CH0_CCR = 0x00000000;  // USIC CH0 is now disabled, idle.

	// For clock = 32MHz:
	// Fractional mode, STEP=0x24E
	// PDIV=0004, DCTQ=15, PCTQ=1 (PDIV=4 for 115200 baud, PDIV=3B for 9600)
	USIC0_CH0_FDR = 0x0000824E;
	USIC0_CH0_BRG = (0x0004 << 16) | (15 << 10) | (1<<8);
	USIC0_CH0_SCTR = 0x07070102;  // 8 bits, active high, passive high
	USIC0_CH0_TCSR = 0x00000500;  // TDSSM=1, TDEN=01
	// majority bit decision, 1 stop, sample at 9
        USIC0_CH0_PCR = (9 << 8) | 1;
	USIC0_CH0_DX3CR = 0x00000000;  // DX3 DXnA selected
	USIC0_CH0_DX0CR = 0x00000006;  // DX0 DXnG selected, fPeriph

	// Configure FIFO.
        // STBI irq enable (on SR1), 32 entries (5), fill limit 1
        USIC0_CH0_TBCTR = (1 << 30) | (1 << 16) | (5 << 24) | (1 << 8);
        // USIC0_CH0_TBCTR = (5 << 24);
	// 32 entries in buffer, DPTR=0x20
	USIC0_CH0_RBCTR = (5 << 24) | 0x20;
	// USIC0_CH0_INPR = (1 << 4);  // Transmit buffer event generates SR1
	USIC0_CH0_PSCR = 0x0001FFFF;  // Clear the protocol status register.
	USIC0_CH0_TRBSCR = 0x0000C707;  // Clear the buffer status register.

	// ASC mode, no hardware control, no parity, transmit buffer irq enabled
	// USIC0_CH0_CCR = (1 << 13) | 2;
	USIC0_CH0_CCR = 2;

	// Configure FIFO interrupt.
	// USIC0.SR1 is interrupt node 10.
	enableInterrupt(10, 128);
}

char ch0TxBuffer[128];
unsigned int ch0TxBufferStart = 0;
unsigned int ch0TxBufferEnd = 0;

static void usicSendCh0(unsigned char c) {
	*USIC0_CH0_IN = (unsigned int)c;
}

void usicBufferSendCh0(const char *msg) {
	unsigned int i;
	for(i=0; msg[i] != '\0'; i++) {
		ch0TxBuffer[i] = msg[i];
	}
	ch0TxBufferEnd = i;
	ch0TxBufferStart = 0;
	usicSendCh0(ch0TxBuffer[ch0TxBufferStart++]);
}

void __attribute__((interrupt("IRQ"))) USIC_SR1(void) {
	// USIC0_CH0_PSR;
	// USIC0_CH0_TRBSR;
	if (ch0TxBufferStart < ch0TxBufferEnd) {
		usicSendCh0(ch0TxBuffer[ch0TxBufferStart++]);
	}
}
