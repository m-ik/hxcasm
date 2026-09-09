.PHONY: all clean test-unit

DEBUG ?= 0

CC := gcc
CFLAGS := -Wall -Wextra -msse4.2
LDFLAGS :=

CXX := g++
CXXFLAGS := -std=c++17 -Isrc/ -Itest/unit -fsanitize=address
GTEST_LDFLAGS := -fsanitize=address -lgtest -lgtest_main -pthread

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG -O0 -g -fsanitize=address
LDFLAGS += -fsanitize=address
else
CFLAGS += -O3
endif

SRC := src/encode.c \
       src/hashtable.c \
       src/hxcasm.c \
       src/parse.c \
       src/symbols.c
OBJ := $(SRC:.c=.o)
BIN := hxcasm

UNIT_SRC := test/unit/test_parse.cpp \
	    test/unit/test_encode.cpp \
	    test/unit/test_symbol_table.cpp
UNIT_OBJ := $(UNIT_SRC:.cpp=.o)
UNIT_BIN := test/unit/run

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

test-unit: $(UNIT_OBJ) $(filter-out src/hxcasm.o, $(OBJ))
	$(CXX) -o $(UNIT_BIN) $^ $(GTEST_LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN) $(UNIT_OBJ) $(UNIT_BIN)
