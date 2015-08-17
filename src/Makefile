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

# List the source files involved in this project
SRCS=init.c ccu.c gpio.c nvic.c systick.c usic.c main.c 
OBJS=$(SRCS:.c=.o)

# The default 'target' (output) is main.elf and it depends on the object files
# being there. These object files are linked together to create main.elf
main.elf : $(OBJS)
	$(LD) $(OBJS) $(LIBSPEC) -T linker_script.ld -lc --cref -Map main.map -nostartfiles -o main.elf
	arm-none-eabi-objcopy -O binary main.elf main.bin
	objcopy -O ihex main.elf main.hex
	@echo "done"

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

# if someone types in 'make clean' then remove all object files and executables
# associated with this project
clean: 
	rm $(OBJS) main.elf main.bin 
