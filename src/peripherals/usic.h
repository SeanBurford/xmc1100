// Support for the XMC1100 USIC (universal serial interface channel)

#ifndef PERIPHERALS_USIC_H
#define PERIPHERALS_USIC_H

enum usic_protocols {
        USIC_PROTO_DISABLED = 0,
        USIC_PROTO_SSC = 1,
        USIC_PROTO_ASC = 2,
        USIC_PROTO_IIS = 3,
        USIC_PROTO_IIC = 4,
};

unsigned int usicEnable(void);
// Eg. usicConfigure(0, USIC_PROTO_ASC);
unsigned int usicConfigure(int channel, int protocol);
void usicSendCh0(void);
extern void __attribute__((weak)) usicCh0Receive(unsigned int);
extern unsigned int __attribute__((weak)) usicCh0TransmitDone(void);
extern unsigned short __attribute((weak)) usicCh0Transmit(void);


#define USIC0_CCFG(cbase)   REGISTER_32(cbase + 0x004)

#define USIC0_KSCFG(cbase)  REGISTER_32(cbase + 0x00c)
#define USIC0_FDR(cbase)    REGISTER_32(cbase + 0x010)
#define USIC0_BRG(cbase)    REGISTER_32(cbase + 0x014)
#define USIC0_INPR(cbase)   REGISTER_32(cbase + 0x018)
#define USIC0_DX0CR(cbase)  REGISTER_32(cbase + 0x01c)
#define USIC0_DX1CR(cbase)  REGISTER_32(cbase + 0x020)
#define USIC0_DX2CR(cbase)  REGISTER_32(cbase + 0x024)
#define USIC0_DX3CR(cbase)  REGISTER_32(cbase + 0x028)
#define USIC0_DX4CR(cbase)  REGISTER_32(cbase + 0x02c)
#define USIC0_DX5CR(cbase)  REGISTER_32(cbase + 0x030)
#define USIC0_SCTR(cbase)   REGISTER_32(cbase + 0x034)
#define USIC0_TCSR(cbase)   REGISTER_32(cbase + 0x038)
#define USIC0_PCR(cbase)    REGISTER_32(cbase + 0x03c)
#define USIC0_CCR(cbase)    REGISTER_32(cbase + 0x040)

#define USIC0_PSCR(cbase)   REGISTER_32(cbase + 0x04c)

#define USIC0_TBCTR(cbase)  REGISTER_32(cbase + 0x108)
#define USIC0_RBCTR(cbase)  REGISTER_32(cbase + 0x10c)

#define USIC0_TRBSCR(cbase) REGISTER_32(cbase + 0x118)

#endif
