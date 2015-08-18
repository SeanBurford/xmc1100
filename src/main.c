#include "xmc1100.h"
#include "ccu.h"
#include "gpio.h"
#include "usic.h"
#include "usic_buffer.h"
#include "systick.h"

unsigned int postReset(void)
{
	// 12.4.2 Reset Status
	unsigned int status;
	status = SCU_RSTSTAT & 0x3FF;
	SCU_RSTCLR = BIT0;
	// Reset on ECC error, loss of clock.
	SCU_RSTCON = BIT1 | BIT0;
	return status;
}

void configureClock(void)
{
	SCU_PASSWD = 0xC0;
	// CNTADJ = 1024 clock cycles
	// RTC gets clock from standby source
	// PCLK = 2 * MCLK (64MHz) (fdiv=00/256, idiv=01 (32MHz))
	// MCLK = 32MHz
	SCU_CLKCR = 0x3FF10100;
	// SCU_CLKCR = 0x3ff00400;  // 8MHz
	while (SCU_CLKCR & (BIT30 | BIT31)); // wait for vccp to stabilise.
	SCU_PASSWD = 0x00C3;
}

int main()
{
	disable_interrupts();
	postReset();
	configureClock();

        enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
        enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
        enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	usicEnable();
	usicConfigureCh0();

	ccuEnable();
	ccuConfigureSlice0();
	ccuConfigureSlice1();
	// CCU40 OUT0 is connected to P0.0, P0.5, P0.6, P1.0, P2.0.
        // Set P0.7 to pull up input (wire up to P0.6 CCU4.OUT0)
	// P0.7 is configured in ccuConfigureSlice1 as the capture trigger.
	enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	enablePin(0, 6, GPIO_OUT_OD_ALT4);  // P0.6 alt4 is CCU4.OUT0
        enablePin(0, 7, GPIO_IN_PU);

	systickEnable();

	enable_interrupts();

	ccuStartSlices();

	usicBufferSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}

	return 0;
}
