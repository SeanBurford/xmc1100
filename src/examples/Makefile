CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..
LDFLAGS=-T ../linker.flash.ld --cref -nostartfiles
LIBC=-L /usr/lib/arm-none-eabi/newlib/armv6-m -lc

all:	ultrasonic.elf speed_test.elf temp.elf
	@echo "done"

MAIN_LIBS=../init.o ../peripherals/scu.o \
          ../peripherals/ccu.o ../peripherals/nvic.o ../peripherals/usic.o \
          ../peripherals/gpio.o ../peripherals/systick.o \
          usic_buffer.o
ultrasonic.elf:	$(MAIN_LIBS) ultrasonic.o
	$(LD) $(LDFLAGS) -Map ultrasonic.map $^ -o $@
	arm-none-eabi-objcopy -O binary $@  ultrasonic.bin
	objcopy -O ihex $@  ultrasonic.hex

SPEED_TEST_LIBS=../init.o ../peripherals/scu.o \
                ../peripherals/gpio.o ../peripherals/nvic.o \
                ../peripherals/usic.o
speed_test.elf:	$(SPEED_TEST_LIBS) speed_test.o
	$(LD) $(LDFLAGS) -Map speed_test.map $^ -o $@

TEMPERATURE_LIBS=../init.o ../peripherals/ccu.o \
                 ../peripherals/scu.o ../peripherals/gpio.o \
                 ../peripherals/nvic.o ../peripherals/systick.o \
                 ../peripherals/usic.o usic_buffer.o \
                 ../peripherals/temperature.o
temp.elf:	$(TEMPERATURE_LIBS) temp.o
	$(LD) $(LDFLAGS) -Map temp.map $^ -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map *.bin *.hex
