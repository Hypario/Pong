SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

%.o: %.c
		gcc -o $@ $< -lncurses

all: $(OBJS)

clean:
	rm *.o
