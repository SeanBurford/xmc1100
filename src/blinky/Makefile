CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..
LDFLAGS=-T ../linker.flash.ld --cref -nostartfiles
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld

ELF=blinky_cpu.elf \
    blinky_systick.elf \
    blinky_ccu.elf \
    blinky_rtc.elf \
    blinky_rtc_deepsleep.elf \
    blinky_wdt.elf

all:	$(ELF)
	@echo "done"

CPU_LIBS=../peripherals/init.o ../peripherals/scu.o ../peripherals/gpio.o
blinky_cpu.elf:	$(CPU_LIBS) blinky_cpu.o
	$(LD) $(LDFLAGS) -Map blinky_cpu.map $^ -o $@

SYSTICK_LIBS=$(CPU_LIBS) ../peripherals/systick.o
blinky_systick.elf:	$(SYSTICK_LIBS) blinky_systick.o
	$(LD) $(LDFLAGS) -Map blinky_systick.map $^ -o $@

CCU_LIBS=$(CPU_LIBS) ../peripherals/ccu.o ../peripherals/nvic.o
blinky_ccu.elf:	$(CCU_LIBS) blinky_ccu.o
	$(LD) $(LDFLAGS) -Map blinky_ccu.map $^ -o $@

RTC_LIBS=$(CPU_LIBS) ../peripherals/rtc.o ../peripherals/nvic.o
blinky_rtc.elf:	$(RTC_LIBS) blinky_rtc.o
	$(LD) $(LDFLAGS) -Map blinky_rtc.map $^ -o $@

blinky_rtc_deepsleep.elf:	$(RTC_LIBS) blinky_rtc_deepsleep.o
	$(LD) $(LDFLAGS) -Map blinky_rtc_deepsleep.map $^ -o $@

WDT_LIBS=$(CPU_LIBS) ../peripherals/nvic.o ../peripherals/wdt.o
blinky_wdt.elf:	$(WDT_LIBS) blinky_wdt.o
	$(LD) $(LDFLAGS) -Map blinky_wdt.map $^ -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map
