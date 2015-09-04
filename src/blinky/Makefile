CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld

ELF=blinky_cpu.elf \
    blinky_systick.elf \
    blinky_ccu.elf \
    blinky_rtc.elf \
    blinky_rtc_deepsleep.elf

all:	$(ELF)
	@echo "done"

CPU_LIBS=../peripherals/init.o ../peripherals/scu.o ../peripherals/gpio.o
blinky_cpu.elf:	$(CPU_LIBS) blinky_cpu.o
	$(LD) $(CPU_LIBS) blinky_cpu.o \
	      -T ../linker_script.ld --cref \
	      -Map blinky_cpu.map -nostartfiles -o $@

SYSTICK_LIBS=$(CPU_LIBS) ../peripherals/systick.o
blinky_systick.elf:	$(SYSTICK_LIBS) blinky_systick.o
	$(LD) $(SYSTICK_LIBS) blinky_systick.o \
	      -T ../linker_script.ld --cref \
	      -Map blinky_systick.map -nostartfiles -o $@

CCU_LIBS=$(CPU_LIBS) ../peripherals/ccu.o ../peripherals/nvic.o
blinky_ccu.elf:	$(CCU_LIBS) blinky_ccu.o
	$(LD) $(CCU_LIBS) blinky_ccu.o \
	      -T ../linker_script.ld --cref \
	      -Map blinky_ccu.map -nostartfiles -o $@

RTC_LIBS=$(CPU_LIBS) ../peripherals/rtc.o ../peripherals/nvic.o
blinky_rtc.elf:	$(RTC_LIBS) blinky_rtc.o
	$(LD) $(RTC_LIBS) blinky_rtc.o \
	      -T ../linker_script.ld --cref \
	      -Map blinky_rtc.map -nostartfiles -o $@

blinky_rtc_deepsleep.elf:	$(RTC_LIBS) blinky_rtc_deepsleep.o
	$(LD) $(RTC_LIBS) blinky_rtc_deepsleep.o \
	      -T ../linker_script.ld --cref \
	      -Map blinky_rtc_deepsleep.map -nostartfiles -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map
