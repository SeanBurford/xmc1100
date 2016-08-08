#include "xmc1100.h"
#include "peripherals/nvic.h"
#include "peripherals/usic.h"

#include "usic_fifo.h"

unsigned int usicFifoEnable(int channel) {
	const unsigned int cbase = usicChannelBase(channel);
	if (cbase == 0)
		return 1;

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

	// Clear the protocol status register.
	USIC0_PSCR(cbase) = 0x0001FFFF;

	// Configure FIFO interrupts.
	// USIC0.SR0 (RX FIFO) is interrupt node 9.
	// USIC0.SR1 (TX FIFO) is interrupt node 10.
	enableInterrupt(9, 129);
	enableInterrupt(10, 128);

	return 0;
}

void __attribute__((interrupt("IRQ"))) USIC_SR0(void) {
	// Configured as channel 0 FIFO RX by RBCTR (uses usic.h callbacks).

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

void __attribute__((interrupt("IRQ"))) USIC_SR1(void) {
	// Send bytes via FIFO (uses usic.h callbacks).

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

