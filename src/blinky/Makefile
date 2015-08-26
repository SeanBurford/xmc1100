LIBSPEC=-L /usr/lib/arm-none-eabi/newlib/armv6-m
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld

all:	blinky_cpu.elf blinky_systick.elf
	@echo "done"

CPU_LIBS=../peripherals/init.o ../peripherals/scu.o ../peripherals/gpio.o
blinky_cpu.elf:	$(CPU_LIBS) blinky_cpu.o
	$(LD) $(CPU_LIBS) blinky_cpu.o $(LIBSPEC) \
	      -T ../linker_script.ld -lc --cref \
	      -Map blinky_cpu.map -nostartfiles -o $@

SYSTICK_LIBS=$(CPU_LIBS) ../peripherals/systick.o
blinky_systick.elf:	$(SYSTICK_LIBS) blinky_systick.o
	$(LD) $(SYSTICK_LIBS) blinky_systick.o $(LIBSPEC) \
	      -T ../linker_script.ld -lc --cref \
	      -Map blinky_systick.map -nostartfiles -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map
