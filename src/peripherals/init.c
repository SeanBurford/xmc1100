// Initialization and interrupt vectors for the infineon 
// XMC1100 microcontroller.
// No claims are made as to the suitability of this code for any 
// purpose whatsoever.  Use at your own risk!
// Does not include initialization for C++ global/static objects
// so probably best to use this with C projects for now.
// Latest version of this code can be found by visiting
// http://eleceng.dit.ie/frank and follow the links
// Author: Frank Duignan

static void init(void);
extern int main();

// The following are 'declared' in the linker script
extern unsigned int INIT_DATA_VALUES;
extern unsigned int INIT_DATA_START;
extern unsigned int INIT_DATA_SIZE;
extern unsigned int BSS_START;
extern unsigned int BSS_SIZE;

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

// Weak interrupt handler definitions.  Declaring another function of the
// same name will override the default unhandledIRQ handler for that interrupt.
void __attribute__((interrupt("IRQ"))) unhandledIRQ(void) {
	static unsigned int unhandled_interrupts;
	unhandled_interrupts++;
}

void __attribute__((interrupt("IRQ"))) hardfaultDefault(void) {
	while(1) {
		asm("wfi");
	}
}

void __attribute__((weak, alias("hardfaultDefault"))) hardfaultHandler(void);
void __attribute__((weak, alias("unhandledIRQ"))) systickHandler(void);
void __attribute__((weak, alias("unhandledIRQ"))) SCU_SR0(void);
void __attribute__((weak, alias("unhandledIRQ"))) SCU_SR1(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ02(void);
void __attribute__((weak, alias("unhandledIRQ"))) ERU_SR0(void);
void __attribute__((weak, alias("unhandledIRQ"))) ERU_SR1(void);
void __attribute__((weak, alias("unhandledIRQ"))) ERU_SR2(void);
void __attribute__((weak, alias("unhandledIRQ"))) ERU_SR3(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ07(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ08(void);
void __attribute__((weak, alias("unhandledIRQ"))) USIC_SR0(void);
void __attribute__((weak, alias("unhandledIRQ"))) USIC_SR1(void);
void __attribute__((weak, alias("unhandledIRQ"))) USIC_SR2(void);
void __attribute__((weak, alias("unhandledIRQ"))) USIC_SR3(void);
void __attribute__((weak, alias("unhandledIRQ"))) USIC_SR4(void);
void __attribute__((weak, alias("unhandledIRQ"))) USIC_SR5(void);
void __attribute__((weak, alias("unhandledIRQ"))) VADC0_SR0(void);
void __attribute__((weak, alias("unhandledIRQ"))) VADC0_SR1(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ17(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ18(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ19(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ20(void);
void __attribute__((weak, alias("unhandledIRQ"))) CCU40_SR0(void);
void __attribute__((weak, alias("unhandledIRQ"))) CCU40_SR1(void);
void __attribute__((weak, alias("unhandledIRQ"))) CCU40_SR2(void);
void __attribute__((weak, alias("unhandledIRQ"))) CCU40_SR3(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ25(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ26(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ27(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ28(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ29(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ30(void);
void __attribute__((weak, alias("unhandledIRQ"))) IRQ31(void);

// The remaining interrupt vectors are relocated to RAM where a jump
// table should be placed to the actual interrupt handlers.  The jump 
// table takes the following form:
//	ldr R0,=handler_address
//  mov PC,R0
// This pair of instructions fits into 4 bytes which
// fits neatly into a jump table entry.
inline void JumpTable(void) __attribute__(( section(".remapped_vectors")));
inline void JumpTable(void) 
{
	asm(" .long 0 "); // -15 Power up and warm reset
	asm(" .long 0 "); // -14 NMI
	asm(" ldr R0,=(hardfaultHandler) "); // -13 HardFault handler
	asm(" mov PC,R0");
	asm(" .long 0 "); // -12 reserved
	asm(" .long 0 "); // -11 reserved
	asm(" .long 0 "); // -10 reserved
	asm(" .long 0 "); // -9 reserved
	asm(" .long 0 "); // -8 reserved
	asm(" .long 0 "); // -7 reserved
	asm(" .long 0 "); // -6 reserved
	asm(" .long 0 "); // -5 SVCall
	asm(" .long 0 "); // -4 Debug monitor 
	asm(" .long 0 "); // -3 reserved 
	asm(" .long 0 "); // -2 SVCall
	asm(" ldr R0,=(systickHandler) "); // -1 Systick handler
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(SCU_SR0) ");     // 00: System Control SR0 (critical)
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(SCU_SR1) ");     // 01: System Control SR1 (common)
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ02) ");      // 02: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(ERU_SR0) ");    // 03
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(ERU_SR1) ");    // 04
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(ERU_SR2) ");    // 05
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(ERU_SR3) ");    // 06
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ07) ");      // 07: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ08) ");      // 08: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(USIC_SR0) ");   // 09
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(USIC_SR1) ");   // 10
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(USIC_SR2) ");   // 11
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(USIC_SR3) ");   // 12
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(USIC_SR4) ");   // 13
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(USIC_SR5) ");   // 14
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(VADC0_SR0) ");  // 15
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(VADC0_SR1) ");  // 16
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ17) ");      // 17: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ18) ");      // 18: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ19) ");      // 19: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ20) ");      // 20: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(CCU40_SR0) ");  // 21
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(CCU40_SR1) ");  // 22
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(CCU40_SR2) ");  // 23
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(CCU40_SR3) ");  // 24
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ25) ");      // 25: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ26) ");      // 26: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ27) ");      // 27: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ28) ");      // 28: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ29) ");      // 29: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ30) ");      // 30: Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ31) ");      // 31: Reserved
	asm(" mov PC,R0 ");
};

static void init()
{
	// do global/static data initialization
	// This is will also copy the jump table for remapped IRQ vectors
	// to RAM.
	const unsigned int *src = (unsigned int *)&INIT_DATA_VALUES;
	unsigned int *dest = (unsigned int *)&INIT_DATA_START;
	int len;
	for(len=(int)&INIT_DATA_SIZE / 4; len >= 0; len--)
		dest[len] = src[len];

	// zero out the uninitialized global/static variables
	unsigned int *const bss = &BSS_START;
	for(len=(int)&BSS_SIZE / 4; len >= 0; len--)
		bss[len] = 0;

	main();
}

