CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..
LDFLAGS=-T ../linker.flash.ld -nostartfiles
LDFLAGS_SRAM=-T ../linker.sram.ld -nostartfiles
LIBC=-L /usr/lib/arm-none-eabi/newlib/armv6-m -lc

all:	ultrasonic.elf speed_test.elf temp.elf two_phase_pwm.elf eru.elf
	@echo "done"

MAIN_LIBS=../init.o ../peripherals/scu.o \
          ../peripherals/ccu.o ../peripherals/nvic.o ../peripherals/usic.o \
          ../peripherals/gpio.o ../peripherals/systick.o \
          usic_buffer.o
ultrasonic.elf:	$(MAIN_LIBS) ultrasonic.o
	$(LD) $(LDFLAGS) --cref -Map $@.map $^ -o $@
	arm-none-eabi-objcopy -O binary $@ ultrasonic.bin
	objcopy -O ihex $@  ultrasonic.hex

two_phase_pwm.elf:	$(MAIN_LIBS) two_phase_pwm.o
	$(LD) $(LDFLAGS) --cref -Map $@.map $^ -o $@
	arm-none-eabi-objcopy -O binary $@ two_phase_pwm.bin
	objcopy -O ihex $@  two_phase_pwm.hex

eru.elf:	$(MAIN_LIBS) ../peripherals/eru.o eru.o
	$(LD) $(LDFLAGS_SRAM) --cref -Map $@.map $^ -o $@
	arm-none-eabi-objcopy -O binary $@ eru.bin
	objcopy -O ihex $@  eru.hex

SPEED_TEST_LIBS=../init.o ../peripherals/scu.o \
                ../peripherals/gpio.o ../peripherals/nvic.o \
                ../peripherals/usic.o
speed_test.elf:	$(SPEED_TEST_LIBS) speed_test.o
	$(LD) $(LDFLAGS) --cref -Map $@.map $^ -o $@

TEMPERATURE_LIBS=../init.o ../peripherals/ccu.o \
                 ../peripherals/scu.o ../peripherals/gpio.o \
                 ../peripherals/nvic.o ../peripherals/systick.o \
                 ../peripherals/usic.o usic_buffer.o \
                 ../peripherals/temperature.o
temp.elf:	$(TEMPERATURE_LIBS) temp.o
	$(LD) $(LDFLAGS) --cref -Map $@.map $^ -o $@

.c.o:	
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm *.o *.elf *.map *.bin *.hex
