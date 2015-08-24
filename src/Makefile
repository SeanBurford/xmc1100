# In order to use this makefile you must have an arm compiler on your
# system. You can get one here: https://launchpad.net/gcc-arm-embedded
# Your PATH environment variable should include the 'bin' folder in
# the arm compiler directory tree.

# You must edit the LIBSPEC variable below to point to your arm compiler.
# A particular version of the compiler is mentioned in the LIBSPEC 
# line below.  Find out what the equivalent is for your system and change
# it accordingly.
# Tell the linker where to find the libraries -> important: use thumb versions
LIBSPEC=-L /usr/lib/arm-none-eabi/newlib/armv6-m
CCFLAGS=-c -mcpu=cortex-m0 -mthumb -g -Wall -Werror

# Specify the compiler, assembler and linker to use
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld

all:	main.elf speed_test.elf
	@echo "done"

MAIN_LIBS=peripherals/init.o peripherals/scu.o \
          peripherals/ccu.o peripherals/nvic.o peripherals/usic.o \
          peripherals/gpio.o peripherals/systick.o \
          usic_buffer.o
main.elf:	$(MAIN_LIBS) main.o
	$(LD) $(MAIN_LIBS) main.o $(LIBSPEC) -T linker_script.ld -lc --cref \
	      -Map main.map -nostartfiles -o main.elf
	arm-none-eabi-objcopy -O binary main.elf main.bin
	objcopy -O ihex main.elf main.hex

SPEED_TEST_LIBS=peripherals/init.o peripherals/scu.o \
                peripherals/gpio.o peripherals/nvic.o peripherals/usic.o
speed_test.elf:	$(SPEED_TEST_LIBS) speed_test.o
	$(LD) $(SPEED_TEST_LIBS) speed_test.o \
	      $(LIBSPEC) -T linker_script.ld -lc --cref -Map speed_test.map \
	      -nostartfiles -o $@

main.o:	main.c
	$(CC) -c $(CCFLAGS) $< -o $@

speed_test.o:	speed_test.c
	$(CC) -c $(CCFLAGS) $< -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf main.bin main.hex main.map
