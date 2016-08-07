CC=arm-none-eabi-gcc
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror -I..

SRCS=adc.c \
     ccu.c \
     eru.c \
     gpio.c \
     nvic.c \
     rtc.c \
     scu.c \
     systick.c \
     temperature.c \
     usic.c \
     usic_fifo.c \
     wdt.c
OBJS=$(SRCS:.c=.o)

all:	$(OBJS)
	@echo "peripherals done"

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm -f *.o
