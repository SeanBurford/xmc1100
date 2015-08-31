#include "peripherals/xmc1100.h"
#include "peripherals/ccu.h"
#include "peripherals/gpio.h"
#include "peripherals/scu.h"

static void blink(void) {
	// On suspend wait for rollover then clamp the outputs.
	// By default the PWM keeps running even in suspend (eg. debug).
	ccuEnable(GCTRL_SUSCFG_ROLLOVER);
	ccuConfigureSlice0(
	    // Reset count and start on event 0 (Input I: SCU CCU start event)
	    EV0IS_INyI | EV0EM_RISING, STRTS_EV0,
	    CMOD_COMPARE | CLST_ENABLE | STRM_BOTH,
	    PSC_FCCU_512,  // Prescaler 8MHz / 512 = 15,625Hz
	    31250, 15625,  // 0.5Hz 50%
	    0, 0, 0);      // Generate no interrupts. passive level low.
	ccuConfigureSlice1(
	    // Reset count and start on event 0 (Input I: SCU CCU start event)
	    EV0IS_INyI | EV0EM_RISING, STRTS_EV0,
	    CMOD_COMPARE | CLST_ENABLE | STRM_BOTH,
	    PSC_FCCU_512,  // Prescaler 8MHz / 512 = 15,625Hz
	    31250, 15625,  // 0.5Hz 50%
	    0, 0, 1);      // Generate no interrupts. passive level high.
	enablePin(1, 0, GPIO_OUT_PP_ALT2);  // LED P1.0 alt2 is CCU4.OUT0
	enablePin(1, 1, GPIO_OUT_PP_ALT2);  // LED P1.1 alt2 is CCU4.OUT1
	ccuStartSlices(BIT0 | BIT1);
}

int main() {
	scuPostReset(CLKCR_M8_P8);
        enable_interrupts();
	blink();
        while(1) {
                asm("wfi");
        }
	return 0;
}

