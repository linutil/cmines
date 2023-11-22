# cmines makefile
SRC=$(wildcard src/*.c)
HDR=$(wildcard src/*.h)
OBJ=$(patsubst %.c,%.o,${SRC})

CC:=gcc
LD:=gcc

LIB:=-lraylib
CFLAGS:=-std=c99
LDFLAGS:=

%.o: %.c ${HDR}
	$(CC) $(CFLAGS) -c $< -o $@

cmines: ${OBJ}
	$(LD) $(LDFLAGS) $^ -o cmines $(LIB)

all: cmines

run: all
	.c/mines

clean:
	$(RM) -f ${OBJ}

cleandist: clean
	$(RM) -f cmines
