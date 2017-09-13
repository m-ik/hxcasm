DEBUG = 1

CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = 

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG -O0 -g
else
CFLAGS += -O3
endif

SRC = utils.c dict.c hxcasm.c
OBJ = ${SRC:.c=.o}
BIN = hxcasm

all: clean $(BIN)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean

.SUFFIXES: .c .o
