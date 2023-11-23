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
CHMOD:=chmod
ECHO:=echo

BINARY_NAME:=cmines

INSTALL_DIRECTORY:=/opt/cmines
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
	$(RM) -rf $(INSTALL_DIRECTORY)
	$(MKDIR) $(INSTALL_DIRECTORY)
	$(CP) -r $(BINARY_NAME) assets $(INSTALL_DIRECTORY)
	$(ECHO) "pushd $(INSTALL_DIRECTORY) && ./$(BINARY_NAME) && popd" >> $(INSTALL_DIRECTORY)/run.sh
	$(CHMOD) +x $(INSTALL_DIRECTORY)/run.sh

	$(RM) $(BINARY_LD)
	$(LN) -s $(INSTALL_DIRECTORY)/run.sh $(BINARY_LD)

	$(CP) cmines.desktop /usr/share/applications/cmines.desktop

uninstall:
	$(RM) -rf $(INSTALL_DIRECTORY) $(BINARY_LD) /usr/share/applications/cmines.desktop