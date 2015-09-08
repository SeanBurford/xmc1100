CC=arm-none-eabi-gcc
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror

SRCS=init.c adc.c ccu.c gpio.c nvic.c rtc.c scu.c systick.c temperature.c usic.c
OBJS=$(SRCS:.c=.o)

all:	$(OBJS)
	@echo "peripherals done"

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm -f *.o
