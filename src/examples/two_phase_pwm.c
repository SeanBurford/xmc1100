// Drive an ultrasonic transducer using the capture compare unit.

#include "xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/gpio.h"
#include "peripherals/nvic.h"
#include "peripherals/scu.h"
#include "peripherals/systick.h"
#include "usic_buffer.h"

unsigned int reset_reason = 0;

void configureCCU()
{
	// Capture compare unit config
	ccuEnable(GCTRL_SUSCFG_ROLLOVER);
	// Slice 0: 40kHz 50% PWM.
	// Event 1: active high, rising edge, input I (SCU)
	// Clear the timer (STRM) and start on event 1
	// Transfer shadow registers on timer clear
	ccuConfigureSlice0(EV1IS_INyI | EV1EM_RISING,
			   STRTS_EV1,
			   CMOD_COMPARE | TCM_CENTER | CLST_ENABLE | STRM_BOTH,
			   PSC_FCCU_8,  // Prescaler: 64MHz / 8 = 8MHz
			   99, 5,  // 40kHz (center aligned), 5%
			   0, 0,    // No interrupts
			   0);      // Passive level low
	// Slice 1: 40kHz 50% PWM (inverted output).
	// Event 1: active high, rising edge, input I (SCU.GSC40)
	// Clear the timer (STRM) and start on event 1.
	ccuConfigureSlice1(EV1IS_INyI | EV1EM_RISING,
	                   STRTS_EV1,
	                   CMOD_COMPARE | TCM_CENTER | CLST_ENABLE | STRM_BOTH,
	                   PSC_FCCU_8,  // Prescaler: 64MHz / 8 = 8MHz
	                   99, 95,  // 40kHz (center aligned), 5%
	                   0, 0,    // No interrupts
	                   1);  // Passive level high
	// CCU40 OUT0 is available at P0.0, P0.5, P0.6, P1.0, P2.0.
	// P0.6: push pull PWM output. (CCU4.OUT0)
	// P0.7: push pull PWM output (CCU4.OUT1).
	// P1.0: push pull PWM output (LED). (CCU4.OUT0)
	// enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	enablePin(0, 6, GPIO_OUT_PP_ALT4);  // P0.6 alt4 is CCU4.OUT0
	enablePin(0, 7, GPIO_OUT_PP_ALT4);  // P0.7 alt4 is CCU4.OUT1
}

int main()
{
	reset_reason = scuPostReset(CLKCR_M32_P64);

	enablePin(1, 0, GPIO_OUT_PP);  // LED
	enablePin(1, 1, GPIO_OUT_PP);  // LED
	enablePin(2, 1, GPIO_OUT_PP_ALT6);  // P2.1 alt6 is USIC0_CH0_DOUT0
	enablePin(2, 2, GPIO_IN_FLOAT);  // P2.2 is the debug serial input

	usicBufferEnable();
	configureCCU();
	// Clock = 64MHz so 8000000 - 1 is 8 systicks/second.
	systickEnable(8000000 - 1);
	enable_interrupts();

	// Start CCU slices 0 and 1 simultaneously.
	ccuStartSlices(BIT1 | BIT0);

	usicBufferSendCh0("Ready.\r\n");
	while(1)
	{
		asm("wfi");
	}

	return 0;
}

static int current_on_percent = 5;
static int target_on_percent = 5;

void __attribute__((interrupt("IRQ"))) systickHandler(void) {
	togglePinP1(0);
	if (current_on_percent != target_on_percent) {
		setPin(1, 1);
		if (target_on_percent > 50) {
			target_on_percent = 50;
		}
		if (target_on_percent < 0) {
			target_on_percent = 0;
		}
		if (target_on_percent < current_on_percent) {
			current_on_percent -= 1;
			usicBufferSendCh0("-\r\n");
		} else if (target_on_percent > current_on_percent) {
			current_on_percent += 1;
			usicBufferSendCh0("+\r\n");
		}
		ccuSetPeriodCompareSlice0(99, current_on_percent);
		ccuSetPeriodCompareSlice1(99, 100 - current_on_percent);
	} else {
		clearPin(1, 1);
	}
}

// Input character handler.  Echo received characters to output.
void usicCh0Receive(unsigned int val) {
	char msg[2];
	msg[0] = (unsigned char)(val & 0xFF);
	msg[1] = '\0';
	switch(msg[0]) {
	case '0':
		target_on_percent = 0;
		break;
	case '1':
		target_on_percent = 5;
		break;
	case '2':
		target_on_percent = 10;
		break;
	case '3':
		target_on_percent = 15;
		break;
	case '4':
		target_on_percent = 20;
		break;
	case '5':
		target_on_percent = 25;
		break;
	case '6':
		target_on_percent = 30;
		break;
	case '7':
		target_on_percent = 35;
		break;
	case '8':
		target_on_percent = 40;
		break;
	case '9':
		target_on_percent = 45;
		break;
	default:
		;
	}
	usicBufferSendCh0(msg);
}

