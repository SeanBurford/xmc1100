#include "xmc1100.h"
#include "nvic.h"
#include "scu.h"
#include "usic.h"

// Serial inputs from the pins to the protocol processor:
//   DX0,DX3,DX4,DX5 input pins (DX0 for uart, pin select is DXnCR.DSEL).
//   DX1              clock input
//   DX2              control input
// Which feed into registers:
//   RBUF             for byte by byte receive
//   OUTR             for optional FIFO receive

// Serial outputs from the output stage to the pins:
//   DOUT0 ... DOUT3  data out (uart uses DOUT0)
// Which are fed from registers:
//   TBUFx            for byte by byte transmit (includes TCI in register addr)
//   INx & BYP        for optional FIFO transmit (INx includes TCI)

// Interrupt registers:
//   CCR   enables general interrupts
//   INPR  routes general events to usic sr interrupts
//   PSR   indicates the cause of the interrupt
//   PSCR  is the bit clear register for PSR
// General interrupts (with flag and enable+route):
//   Transmit Buffer  TBIF  CCR.TBIEN+INPR.TBINP
//   Receive Buffer   RSIF  CCR.RSIEN+INPR.TBINP (shared with transmit)
//   Transmit Shift   TSIF  CCR.TSIEN+INPR.TSINP
//   Standard Receive RIF   CCR.RIEN+INPR.RINP
//   Alt. Receive     AIF   CCR.RIEN+INPR.RINP
//   Data Lost        DLIF  CCR.DLIEN+INPR.PINP (shared with protocol spec)
//   Baud Rate Gen    BRGIF CCR.BRGIEN+INPR.PINP (shared with protocol spec)
// UART specific interrupts:
//   TX Frame Finish  TFF   PCR.FFIEN
//   RX Frame Finish  RFF
//   Collision Detect COL   PCR.CDEN
//   Syn Break Detect SBD   PCR.SBIEN
//   RX Noise Detect  RNS   PCR.RNIEN
//   Format Err 0     FER0  PCR.FEIEN
//   Format Err 1     FER1 
// FIFO interrupts:
//   TRBSR indicates the cause of the interrupt
//   Standard Tx FIFO event  STBI  TBCTR.STBIEN+TBCTR.STBINP
//   Tx FIFO error event     TBERI TBCTR.SRBIEN+TBCTR.ATBINP
//   Standard Rx FIFO event  SRBI  RBCTR.SRBIEN+RBCTR.SRBINP
//   Alternate Rx FIFO event ARBI  RBCTR.ARBIEN+RBCTR.ARBINP
//   Rx FIFO error event     RBERI RBCTR.RBERIEN+RBCTR.ARBINP

void usicEnable(void) {
	// Clear gating of USIC in SCU_CGATCLR0
	scuUngatePeripheralClock(CGATCLR0_USIC0);
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
	// TDSSM=1: TBUF is considered invalid when moved to shift reg
	// TDEN=1:  TBUF is considered valid when it gets assigned
	USIC0_CH0_TCSR = 0x00000500;  // TDSSM=1, TDEN=01
	// majority bit decision, 1 stop, sample at 9
        USIC0_CH0_PCR = (9 << 8) | 1;
	USIC0_CH0_DX3CR = 0x00000000;  // DX3 DXnA selected (P2.2)
	USIC0_CH0_DX0CR = 0x00000006;  // DX0 DXnG selected, fPeriph

	// Clear the buffer status register and protocol status register.
	USIC0_CH0_TRBSCR = 0x0000C707;
	USIC0_CH0_PSCR = 0x0001FFFF;
	// Configure TX FIFO interrupt characteristics.
        // SIZE: (5<<24): FIFO is 32 entries.
	// STBINP: (1<<16) Standard transmit buffer interrupt node SR1.
	// Trigger mode 1(3<<14): While TRBSR.STBT=1 interrupt until FIFO full.
	// Limit is 1 (1 << 8): Interrupt requests fill at that point.
	// DPTR=0 (lower half of fifo buffer)
        USIC0_CH0_TBCTR = ((5 << 24) | (1 << 16) | (3 << 14) | (1 << 8));
	// Configure RX FIFO interrupt characterstics.
	// SRBIEN=1 (1<<30) generate receive interrupts
	// LOF=1 (1<<28) event when fill level transitions to > 0
	// SIZE=32 (5<<24)
	// SRBINP=0 SR0
	// SRBTEN=0
	// SRBTM=0
	// LIMIT=0
	// DPTR=0x20 top half of FIFO buffer
	USIC0_CH0_RBCTR = (1 << 30) | (1<<28) | (5 << 24) | 0x20;

	// ASC mode, no hardware control, no parity, transmit buffer irq enabled
	// USIC0_CH0_CCR = (1 << 13) | 2;
	USIC0_CH0_CCR = 2;

	// Configure FIFO interrupts.
	// USIC0.SR0 (RX FIFO) is interrupt node 9.
	// USIC0.SR1 (TX FIFO) is interrupt node 10.
	enableInterrupt(9, 129);
	enableInterrupt(10, 128);
}

void __attribute__((interrupt("IRQ"))) USIC_SR0(void) {
	// Configured as channel 0 FIFO RX by RBCTR

	// Check that this is a standard receive buffer event
	if (USIC0_CH0_TRBSR & BIT0) {
		unsigned int level = (USIC0_CH0_TRBSR >> 16) & 0x7F;  // RBFLVL
		while (level > 0) {
			unsigned int c = USIC0_CH0_OUTR;
			if (usicCh0Receive) {
				// Invoke the receive handler.
				usicCh0Receive(c);
			}
			level = (USIC0_CH0_TRBSR >> 16) & 0x7F;
		}
	}

	// Clear RX bits in the transmit/receive buffer status register.
	USIC0_CH0_TRBSCR = BIT2 | BIT1 | BIT0;
}

void usicSendCh0(void) {
	// Enable the TX interrupt and transmit the first byte.
	USIC0_CH0_TBCTR |= BIT30;
	*USIC0_CH0_IN = usicCh0Transmit();
}

void __attribute__((interrupt("IRQ"))) USIC_SR1(void) {
	// SR0 is configured as channel 0 FIFO TX by TBCTR
	// This interrupt will trigger while TRBSR.STBT=1 until the FIFO is full
	// it will also trigger when the FIFO empties to 0.

	// Check that this is a standard transmit buffer event
	if (USIC0_CH0_TRBSR & BIT8) {
		// Check that the TX buffer is not full and we have data to send
		if (!(USIC0_CH0_TRBSR & BIT12) &&
		    usicCh0TransmitDone && usicCh0Transmit &&
		    !usicCh0TransmitDone()) {
			// Transmit a byte.
			*USIC0_CH0_IN = usicCh0Transmit();
		}
	}

	// Disable the TX interrupt when we are done
	if ((!usicCh0TransmitDone) || usicCh0TransmitDone()) {
		USIC0_CH0_TBCTR &= 0xFFFFFFFF ^ BIT30;
	}

	// Clear TX bits in the transmit/receive buffer status register.
	USIC0_CH0_TRBSCR = BIT9 | BIT8;
}
