// Reset trigger enables.
#define RSTCON_ECCRSTEN BIT0
#define RSTCON_LOCRSTEN BIT1
#define RSTCON_SPERSTEN BIT2
#define RSTCON_U0PERSTEN BIT3
#define RSTCON_ALL (BIT0 | BIT1 | BIT2 | BIT3)

// Clock control.
// CNTADJ = 1024 clock cycles
// RTC gets clock from standby source
// PCLK = 2 * MCLK (64MHz) (fdiv=00/256, idiv=01 (32MHz))
// MCLK = 32MHz
#define CLKCR_M32_P64 0x3ff10100
#define CLKCR_M8_P8 0x3ff00400

// Peripheral clock gating controls.
#define CGATCLR0_VADC BIT0
#define CGATCLR0_CCU40 BIT2
#define CGATCLR0_USIC0 BIT3
#define CGATCLR0_WDT BIT9
#define CGATCLR0_RTC BIT10

// Get reason for previous reset.
unsigned int scuResetReason(void);
// Set reset triggers.
void scuResetControl(const unsigned int rstcon);

// Set CPU and peripheral clock speeds.
void scuClockControl(const unsigned int clkcr);

// Enable specified peripheral clock.
void scuUngatePeripheralClock(const unsigned int peripheral);
// Disable specified peripheral clock.
void scuGatePeripheralClock(const unsigned int peripheral);

