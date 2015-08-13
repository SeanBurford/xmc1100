#include "xmc1100.h"
#include "nvic.h"

static volatile unsigned int *nvic_ipr = PTR_32(NVIC_BASE + 0x300);
static const unsigned int nvic_masks[] = {
	0xffffff00, 0xffff00ff, 0xff00ffff, 0x00ffffff
};

void enableInterrupt(unsigned int interrupt, unsigned int priority) {
	const unsigned int ipr_reg = interrupt >> 2;
	const unsigned int ipr_index = interrupt & 0x03;
	priority <<= (ipr_index * 8);
	nvic_ipr[ipr_reg] &= nvic_masks[ipr_index];
	nvic_ipr[ipr_reg] |= priority;
	NVIC_ISER = 1 << interrupt;
}

