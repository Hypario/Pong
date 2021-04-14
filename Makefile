SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LDLIBS?=-lopenal
CFLAGS+=-DLIBAUDIO
LDLIBS+=-lncurses
LDLIBS+=-lpthread
LDLIBS+=-laudio
LDLIBS+=-lalut
%.o: %.c
		gcc -o $@ $< $(LDLIBS) 

all: $(OBJS)

clean:
	rm *.o
