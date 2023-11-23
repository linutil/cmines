# cmines makefile
SRC=$(wildcard src/*.c)
HDR=$(wildcard src/*.h)
OBJ=$(patsubst %.c,%.o,${SRC})

CC:=gcc
LD:=gcc

LIB:=-lraylib -lm
CFLAGS:=-std=c99 -g
LDFLAGS:=-g

MKDIR:=mkdir
CP:=cp
LN:=ln

BINARY_NAME:=cmines

INSTALL_DIRECTORY:=/usr/opt/cmines/
BINARY_LD:=/usr/bin/$(BINARY_NAME)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINARY_NAME): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $(BINARY_NAME) $(LIB)

all: $(BINARY_NAME)

run: all
	./cmines

clean:
	$(RM) -f ${OBJ}

cleandist: clean
	$(RM) -f cmines

install: all
	$(MKDIR) -p $(INSTALL_DIRECTORY)
	$(CP) -r $(BINARY_NAME) assets $(INSTALL_DIRECTORY)
	$(LN) -s $(BINARY_LD) $(INSTALL_DIRECTORY)/$(BINARY_NAME)

