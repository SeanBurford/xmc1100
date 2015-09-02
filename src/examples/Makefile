# In order to use this makefile you must have an arm compiler on your
# system. You can get one here: https://launchpad.net/gcc-arm-embedded
# Your PATH environment variable should include the 'bin' folder in
# the arm compiler directory tree.
# Specify the compiler, assembler and linker to use
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..

all:	ultrasonic.elf speed_test.elf
	@echo "done"

MAIN_LIBS=../peripherals/init.o ../peripherals/scu.o \
          ../peripherals/ccu.o ../peripherals/nvic.o ../peripherals/usic.o \
          ../peripherals/gpio.o ../peripherals/systick.o \
          usic_buffer.o
ultrasonic.elf:	$(MAIN_LIBS) ultrasonic.o
	$(LD) $(MAIN_LIBS) ultrasonic.o \
	      -T ../linker_script.ld --cref \
	      -Map ultrasonic.map -nostartfiles -o ultrasonic.elf
	arm-none-eabi-objcopy -O binary ultrasonic.elf ultrasonic.bin
	objcopy -O ihex ultrasonic.elf ultrasonic.hex

SPEED_TEST_LIBS=../peripherals/init.o ../peripherals/scu.o \
                ../peripherals/gpio.o ../peripherals/nvic.o \
                ../peripherals/usic.o
speed_test.elf:	$(SPEED_TEST_LIBS) speed_test.o
	$(LD) $(SPEED_TEST_LIBS) speed_test.o \
	      -T ../linker_script.ld --cref -Map speed_test.map \
	      -nostartfiles -o $@

ultrasonic.o:	ultrasonic.c
	$(CC) -c $(CCFLAGS) $< -o $@

speed_test.o:	speed_test.c
	$(CC) -c $(CCFLAGS) $< -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map *.bin *.hex
