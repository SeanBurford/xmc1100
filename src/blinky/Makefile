CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..
LDFLAGS=-T ../linker.flash.ld -nostartfiles
LDFLAGS_SRAM=-T ../linker.sram.ld -nostartfiles
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld

ELF=blinky_cpu.elf \
    blinky_systick.elf \
    blinky_ccu.elf \
    blinky_rtc.elf \
    blinky_rtc_deepsleep.elf \
    blinky_wdt.elf
LIBS=../init.o \
     ../peripherals/scu.o \
     ../peripherals/gpio.o \
     ../peripherals/nvic.o


all:	$(ELF)
	@echo "done"

blinky_cpu.elf:	$(LIBS) blinky_cpu.o
	$(LD) $(LDFLAGS) --cref -Map blinky_cpu.map $^ -o $@
	$(LD) $(LDFLAGS_SRAM) $^ -o $@.sram

blinky_systick.elf:	$(LIBS) ../peripherals/systick.o blinky_systick.o
	$(LD) $(LDFLAGS) --cref -Map blinky_systick.map $^ -o $@
	$(LD) $(LDFLAGS_SRAM) $^ -o $@.sram

blinky_ccu.elf:	$(LIBS) ../peripherals/ccu.o blinky_ccu.o
	$(LD) $(LDFLAGS) --cref -Map blinky_ccu.map $^ -o $@

blinky_rtc.elf:	$(LIBS) ../peripherals/rtc.o blinky_rtc.o
	$(LD) $(LDFLAGS) --cref -Map blinky_rtc.map $^ -o $@

blinky_rtc_deepsleep.elf:	$(LIBS) ../peripherals/rtc.o blinky_rtc_deepsleep.o
	$(LD) $(LDFLAGS) --cref -Map blinky_rtc_deepsleep.map $^ -o $@

blinky_wdt.elf:	$(LIBS) ../peripherals/wdt.o blinky_wdt.o
	$(LD) $(LDFLAGS) --cref -Map blinky_wdt.map $^ -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map
