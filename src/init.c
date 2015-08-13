// Initialization and interrupt vectors for the infineon 
// XMC1100 microcontroller.
// No claims are made as to the suitability of this code for any 
// purpose whatsoever.  Use at your own risk!
// Does not include initialization for C++ global/static objects
// so probably best to use this with C projects for now.
// Latest version of this code can be found by visiting
// http://eleceng.dit.ie/frank and follow the links
// Author: Frank Duignan

void init(void);
extern int main();
extern void systickHandler(void);
extern void USIC0_SR0(void);
extern void USIC0_SR1(void);
extern void USIC0_SR2(void);
extern void USIC0_SR3(void);
extern void USIC0_SR4(void);
extern void USIC0_SR5(void);
extern void CCU40_SR0(void);
extern void CCU40_SR1(void);
extern void CCU40_SR2(void);
extern void CCU40_SR3(void);
// The following are 'declared' in the linker script
extern unsigned char  INIT_DATA_VALUES;
extern unsigned char  INIT_DATA_START;
extern unsigned char  INIT_DATA_END;
extern unsigned char  BSS_START;
extern unsigned char  BSS_END;
// The XMC1100 remaps interrupt vectors to RAM.  The boot loader
// looks for the program entry address at 0x10001004 and the
// initial stack pointer as 0x10001000.  These are defined in
// the "vectors" section below.  The section "vectors" is 
// placed at the beginning of flash by the linker script
// Another useful feature of this chip is that you can 
// pre-program startup considtions such as CPU speed and 
// which periperals.  This is done by writing values in to
// CLK_VAL1 and CLK_VAL2 below

const void * FlashVectors[] __attribute__((section(".vectors"))) ={
	(void *)0x20004000, 	/* Top of stack  @0x10001000 */ 
	init, 		 	  	    /* Reset Handler @0x10001004 */
	(void *)0,				/* @0x10001008 */
	(void *)0,				/* @0x1000100c */
	(void *)0xffffffff,		/* CLK_VAL1   @0x10001010 */
	(void *)0xffffffff		/* CLK_VAL2   @0x10001014 */
};
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
	asm(" .long 0 "); // -15 reserved
	asm(" .long 0 "); // -14 reserved
	asm(" .long 0 "); // -13 HardFault
	asm(" .long 0 "); // -12 reserved
	asm(" .long 0 "); // -11 reserved
	asm(" .long 0 "); // -10 reserved
	asm(" .long 0 "); // -9 reserved
	asm(" .long 0 "); // -8 reserved
	asm(" .long 0 "); // -7 reserved
	asm(" .long 0 "); // -6 reserved
	asm(" .long 0 "); // -5 SVCall
	asm(" .long 0 "); // -4 reserved 
	asm(" .long 0 "); // -3 reserved 
	asm(" .long 0 "); // -2 SVCall
	asm(" ldr R0,=(systickHandler) "); // -1 Systick handler
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ00) ");  // System Control SR0 (critical)
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ01) ");  // System Control SR1 (common)
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ02) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ03) ");  // ERU0.SR0
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ04) ");  // ERU0.SR1
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ05) ");  // ERU0.SR2
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ06) ");  // ERU0.SR3
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ07) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ08) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ09) ");  // USIC0.SR0
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ10) ");  // USIC0.SR1
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ11) ");  // USIC0.SR2
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ12) ");  // USIC0.SR3
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ13) ");  // USIC0.SR4
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ14) ");  // USIC0.SR5
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ15) ");  // VADC0.C0SR0
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ16) ");  // VADC0.C0SR1
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ17) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ18) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ19) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ20) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ21) ");  // CCU40.SR0
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ22) ");  // CCU40.SR1
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ23) ");  // CCU40.SR2
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ24) ");  // CCU40.SR3
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ25) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ26) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ27) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ28) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ29) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ30) ");  // Reserved
	asm(" mov PC,R0 ");
	asm(" ldr R0,=(IRQ31) ");  // Reserved
	asm(" mov PC,R0 ");
};

unsigned int irq_counters[32];

void IRQ00(void) {  // System Control SR0 (critical)
	irq_counters[0]++;
}

void IRQ01(void) {  // System Control SR1 (common)
	irq_counters[1]++;
}

void IRQ02(void) {  // Reserved
	irq_counters[2]++;
}

void IRQ03(void) {  // ERU0.SR0
	irq_counters[3]++;
}

void IRQ04(void) {  // ERU0.SR1
	irq_counters[4]++;
}

void IRQ05(void) {  // ERU0.SR2
	irq_counters[5]++;
}

void IRQ06(void) {  // ERU0.SR3
	irq_counters[6]++;
}

void IRQ07(void) {  // Reserved
	irq_counters[7]++;
}

void IRQ08(void) {  // Reserved
	irq_counters[8]++;
}

void IRQ09(void) {  // USIC0.SR0
	irq_counters[9]++;
	USIC0_SR0();
}

void IRQ10(void) {  // USIC0.SR1
	irq_counters[10]++;
	USIC0_SR1();
}

void IRQ11(void) {  // USIC0.SR2
	irq_counters[11]++;
	USIC0_SR2();
}

void IRQ12(void) {  // USIC0.SR3
	irq_counters[12]++;
	USIC0_SR3();
}

void IRQ13(void) {  // USIC0.SR4
	irq_counters[13]++;
	USIC0_SR4();
}

void IRQ14(void) {  // USIC0.SR5
	irq_counters[14]++;
	USIC0_SR5();
}

void IRQ15(void) {  // VADC0.C0SR0
	irq_counters[15]++;
}

void IRQ16(void) {  // VADC0.C0SR1
	irq_counters[16]++;
}

void IRQ17(void) {  // Reserved
	irq_counters[17]++;
}

void IRQ18(void) {  // Reserved
	irq_counters[18]++;
}

void IRQ19(void) {  // Reserved
	irq_counters[19]++;
}

void IRQ20(void) {  // Reserved
	irq_counters[20]++;
}

void IRQ21(void) {  // CCU40.SR0
	irq_counters[21]++;
	CCU40_SR0();
}

void IRQ22(void) {  // CCU40.SR1
	irq_counters[22]++;
	CCU40_SR1();
}

void IRQ23(void) {  // CCU40.SR2
	irq_counters[23]++;
	CCU40_SR2();
}

void IRQ24(void) {  // CCU40.SR3
	irq_counters[24]++;
	CCU40_SR3();
}

void IRQ25(void) {  // Reserved
	irq_counters[25]++;
}

void IRQ26(void) {  // Reserved
	irq_counters[26]++;
}

void IRQ27(void) {  // Reserved
	irq_counters[27]++;
}

void IRQ28(void) {  // Reserved
	irq_counters[28]++;
}

void IRQ29(void) {  // Reserved
	irq_counters[29]++;
}

void IRQ30(void) {  // Reserved
	irq_counters[30]++;
}

void IRQ31(void) {  // Reserved
	irq_counters[31]++;
}

void init()
{
// do global/static data initialization
// This is will also copy the jump table for remapped IRQ vectors
// to RAM.
	unsigned char *src = &INIT_DATA_VALUES;
	unsigned char *dest = &INIT_DATA_START;
	unsigned len= &INIT_DATA_END-&INIT_DATA_START;
	while (len--)
		*dest++ = *src++;
	// zero out the uninitialized global/static variables
	dest = &BSS_START;
	len = &BSS_END - &BSS_START;
	while (len--)
		*dest++=0;

	main();
}

