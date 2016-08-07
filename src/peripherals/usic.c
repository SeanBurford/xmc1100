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

unsigned int usicEnable(void) {
	// Clear gating of USIC in SCU_CGATCLR0
	if (scuUngatePeripheralClock(CGATCLR0_USIC0)) {
		return 1;
	}

	// Check module identification
	// This is not available before peripheral clock starts.
	if ((USIC0_ID >> 8) != 0x0000AAC0) {
		return 1;
	}

	return 0;
}

static unsigned int usicChannelBase(int channel) {
	unsigned int cbase = 0;
	switch(channel) {
	case 0:
		cbase = USIC0_CH0_BASE;
		break;
	case 1:
		cbase = USIC0_CH1_BASE;
		break;
	}
	return cbase;
}

unsigned int usicConfigure(int channel, int protocol) {
	const unsigned int cbase = usicChannelBase(channel);
	if (cbase == 0)
		return 1;

	// Enable USIC module clock and functionality.
	// Module enable (+moden write)
	USIC0_KSCFG(cbase) = BIT1 | BIT0;
	// Set channel operating mode to disabled, idle.
	USIC0_CCR(cbase) = USIC_PROTO_DISABLED;

	// Protocol specific settings.
	unsigned int proto_available = BIT6 | BIT7; // RB, TB buffers
	unsigned int ccr_enable = 0;
	switch(protocol) {
	case USIC_PROTO_ASC:
		proto_available |= BIT1;
		// ASC, no hardware control, no parity.
		ccr_enable = USIC_PROTO_ASC;

		// For clock = 32MHz:
		// Fractional mode, STEP=0x24E
		// PDIV=0004, DCTQ=15, PCTQ=1 (PDIV=4 for 115200 baud,
		//                             PDIV=3B for 9600)
		USIC0_FDR(cbase) = 0x0000824E;
		USIC0_BRG(cbase) = (0x0004 << 16) | (15 << 10) | (1<<8);
		// 8 bits, active high, passive high
		USIC0_SCTR(cbase) = 0x07070102;
		// TDSSM=1: TBUF is considered invalid when moved to shift reg
		// TDEN=1:  TBUF is considered valid when it gets assigned
		USIC0_TCSR(cbase) = 0x00000500;  // TDSSM=1, TDEN=01
		// majority bit decision, 1 stop, sample at 9
		USIC0_PCR(cbase) = (9 << 8) | 1;
		// TODO: This is only correct for CH0 to the XMC2Go pins.
		USIC0_DX3CR(cbase) = 0x00000000;  // DX3 DXnA selected (P2.2)
		USIC0_DX0CR(cbase) = 0x00000006;  // DX0 DXnG selected, fPeriph
		break;
	case USIC_PROTO_IIC:
		proto_available |= BIT2;
		// IIC, parity must be disabled.
		ccr_enable = USIC_PROTO_IIC;
	default:
		return 1;
	}

	// Check that the protocol is supported on this channel.
	if ((USIC0_CCFG(cbase) & proto_available) != proto_available) {
		return 1;
	}

	// TX FIFO interrupt characteristics.
	// SIZE: (4<<24): FIFO is 16 entries.
	// STBINP: (1<<16) Standard transmit buffer interrupt node SR1.
	// Trigger mode 1(3<<14): While TRBSR.STBT=1 interrupt until FIFO full.
	// Limit is 1 (1 << 8): Interrupt requests fill at that point.
	// DPTR=0x10 (0x20 should be the top half of fifo buffer)
	USIC0_TBCTR(cbase) = ((4 << 24) | (1 << 16) | (3 << 14) | (1 << 8) |
	                      0x10);

	// RX FIFO interrupt characterstics.
	// SRBIEN=1 (1<<30) generate receive interrupts
	// LOF=1 (1<<28) event when fill level transitions to > 0
	// SIZE=16 (4<<24)
	// SRBINP=0 SR0
	// SRBTEN=0
	// SRBTM=0
	// LIMIT=0
	// DPTR=0x00 lower half of FIFO buffer
	USIC0_RBCTR(cbase) = (1 << 30) | (1<<28) | (4 << 24) | 0x00;

	// Clear the buffer events and flush the FIFOs.
	// This also loads the output pointers from the input pointers.
	USIC0_TRBSCR(cbase) = 0x0000C707;

	// Generally use default interrupt routing.
	USIC0_INPR(cbase) = 0;

	// Clear the protocol status register.
	USIC0_PSCR(cbase) = 0x0001FFFF;

	// Configure FIFO interrupts.
	// USIC0.SR0 (RX FIFO) is interrupt node 9.
	// USIC0.SR1 (TX FIFO) is interrupt node 10.
	enableInterrupt(9, 129);
	enableInterrupt(10, 128);

	// Enable the protocol unit.
	USIC0_CCR(cbase) = ccr_enable;

	return 0;
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

static void usicSendCh0Byte(void) {
	// Transmit a byte if we have one to send.
	if (!(usicCh0TransmitDone && usicCh0TransmitDone())) {
		if (usicCh0Transmit) {
			USIC0_CH0_IN[0] = usicCh0Transmit();
		} else {
			USIC0_CH0_IN[0] = 'x';
		}
	}
}

void usicSendCh0(void) {
	// Enable the TX interrupt and transmit the first byte.
	USIC0_CH0_TBCTR |= BIT30;
	usicSendCh0Byte();
}

void __attribute__((interrupt("IRQ"))) USIC_SR1(void) {
	// SR0 is configured as channel 0 FIFO TX by TBCTR
	// This interrupt will trigger while TRBSR.STBT=1 until the FIFO is full
	// it will also trigger when the FIFO empties to 0.

	// Check that this is a standard transmit FIFO buffer event
	if (USIC0_CH0_TRBSR & BIT8) {  // BIT8=STBI
		// Clear TX bits in the transmit/receive buffer status register.
		USIC0_CH0_TRBSCR = BIT9 | BIT8;  // BIT8=CSTBI, BIT9=CSTERI

		// Check that the TX buffer is not full.
		if (!(USIC0_CH0_TRBSR & BIT12)) {  // BIT12=TFULL
			usicSendCh0Byte();
		}
	}

	// Disable the TX interrupt when we are done
	if ((!usicCh0TransmitDone) || usicCh0TransmitDone()) {
		USIC0_CH0_TBCTR &= 0xFFFFFFFF ^ BIT30;  // BIT30=STBIEN
	}
}
