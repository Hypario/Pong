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

pong:
	gcc -o main.o $(LDLIBS) main.c -Wno-deprecated-declarations

all: pong

pong_nosound:
	gcc -o main.o main_noOpenAL.c -lncurses -lpthread

clean:
	rm *.o
