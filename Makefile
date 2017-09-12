CC = gcc
CFLAGS = -Wall -Wextra -O3 -g
LDFLAGS = 

SRC = utils.c dict.c hxcasm.c
OBJ = ${SRC:.c=.o}
EXEC = hxcasm

all: clean $(EXEC)

.c.o:
	$(CC) $(CFLAGS) -c $<

hxcasm: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ) $(EXEC)
