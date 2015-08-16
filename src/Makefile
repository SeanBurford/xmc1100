# In order to use this makefile you must have an arm compiler on your
# system. You can get one here: https://launchpad.net/gcc-arm-embedded
# Your PATH environment variable should include the 'bin' folder in
# the arm compiler directory tree.
# One final thing.  You must edit the LIBSPEC variable below to make
# it point at your arm compiler.
# A particular version of the compiler is mentioned in the LIBSPEC 
# line below.  Find out what the equivalent is for your system and change
# it accordingly.
# Tell the linker where to find the libraries -> important: use thumb versions
LIBSPEC=-L /usr/lib/arm-none-eabi/newlib/armv6-m

# Specify the compiler to use
CC=arm-none-eabi-gcc
# Specify the assembler to use
AS=arm-none-eabi-as
# Specity the linker to use
LD=arm-none-eabi-ld

CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror

# List the object files involved in this project
OBJS=	init.o \
	ccu.o \
	gpio.o \
	nvic.o \
	systick.o \
	usic.o \
	main.o 

# The default 'target' (output) is main.elf and it depends on the object files being there.
# These object files are linked together to create main.elf
main.elf : $(OBJS)
	$(LD) $(OBJS) $(LIBSPEC) -T linker_script.ld -lc --cref -Map main.map -nostartfiles -o main.elf
	arm-none-eabi-objcopy -O binary main.elf main.bin
	objcopy -O ihex main.elf main.hex
	@echo "done"
	
# The object file main.o depends on main.c.  main.c is compiled to make main.o
main.o: main.c ccu.h usic.h systick.h
	$(CC) -c $(CCFLAGS) main.c -o main.o

ccu.o: ccu.c ccu.h xmc1100.h nvic.h
	$(CC) -c $(CCFLAGS) ccu.c -o ccu.o

gpio.o: gpio.c gpio.h xmc1100.h
	$(CC) -c $(CCFLAGS) gpio.c -o gpio.o

nvic.o: nvic.c nvic.h xmc1100.h
	$(CC) -c $(CCFLAGS) nvic.c -o nvic.o

systick.o: systick.c systick.h xmc1100.h gpio.h
	$(CC) -c $(CCFLAGS) systick.c -o systick.o

usic.o: usic.c usic.h xmc1100.h nvic.h
	$(CC) -c $(CCFLAGS) usic.c -o usic.o

init.o: init.c
	$(CC) -c $(CCFLAGS) init.c -o init.o

# if someone types in 'make clean' then remove all object files and executables
# associated wit this project
clean: 
	rm $(OBJS) 
	rm main.elf
	rm main.bin 
