CC=arm-none-eabi-gcc
CCFLAGS=-mcpu=cortex-m0 -mthumb -g -Wall -Werror

SRCS=init.c
OBJS=$(SRCS:.c=.o)

all:	$(OBJS)
	@echo "init done"

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm -f *.o
