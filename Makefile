SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

%.o: %.c
		gcc -o $@ $< -lncurses -lpthread

all: $(OBJS)

clean:
	rm *.o
