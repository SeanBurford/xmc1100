/* Initialization and interrupt vectors for the infineon XMC1100 microcontroller
   Based on work by Frank Duignan, modified by Sean Burford.
   http://eleceng.dit.ie/frank/arm/BareMetalXMC2Go/index.html */

extern int main();

// The following are 'declared' in the linker script
extern unsigned int IRQ_DATA_VALUES;
extern unsigned int IRQ_DATA_START;
extern unsigned int IRQ_DATA_SIZE;
extern unsigned int INIT_DATA_VALUES;
extern unsigned int INIT_DATA_START;
extern unsigned int INIT_DATA_SIZE;
extern unsigned int BSS_START;
extern unsigned int BSS_SIZE;

// init() should be the first function in this file so that jumping to
// the start of .text works when running from sram.
static void init()
{
	// Set up stack pointer.
	const unsigned int stack_top = 0x20004000;  // RAM + 16k
	asm("mov r0, %0;"
	    "mov sp, r0;" : : "ir"(stack_top));

	// Do global/static data initialization
	// Copy initialized data and the remapped IRQ jump table vectors to RAM.
	const unsigned int *irq = &IRQ_DATA_VALUES;
	const unsigned int *data = &INIT_DATA_VALUES;
	unsigned int *dest = &IRQ_DATA_START;
	int len;
	for(len=(int)&IRQ_DATA_SIZE / 4; len >= 0; len--)
		dest[len] = irq[len];
	dest = &INIT_DATA_START;
	for(len=(int)&INIT_DATA_SIZE / 4; len >= 0; len--)
		dest[len] = data[len];

	// Zero out the uninitialized global/static variables
	unsigned int *const bss = &BSS_START;
	for(len=(int)&BSS_SIZE / 4; len >= 0; len--)
		bss[len] = 0;

	main();
}

// The XMC1100 remaps interrupt vectors to RAM.  The boot loader
// looks for the program entry address at 0x10001004 and the
// initial stack pointer as 0x10001000.  These are defined in
// the "vectors" section below.  The section "vectors" is
// placed at the beginning of flash by the linker script
// Another useful feature of this chip is that you can
// pre-program startup considtions such as CPU speed and
// which periperals.  This is done by writing values in to
// CLK_VAL1 and CLK_VAL2 below
const void *FlashVectors[] __attribute__((section(".vectors"))) ={
	(void *)0x20004000,	/* Top of stack  0x10001000 */
	init,			/* Reset Handler 0x10001004 */
	(void *)0	,	/* 0x10001008 */
	(void *)0,		/* 0x1000100c */
	(void *)0xffffffff,	/* CLK_VAL1   0x10001010 */
	(void *)0xffffffff	/* CLK_VAL2   0x10001014 */
};


