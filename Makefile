# Install
BIN = json

# Compiler
CC = gcc

#Flags
CFLAGS = -g -Wall -Wextra

.PHONY: clang
clang: CC = clang
clang: $(BIN)

.PHONY: gcc
gcc: CC = gcc
gcc: CFLAGS += -fno-gcse -fno-crossjumping
gcc: $(BIN)

# Objects
SRCS = json.c test.c
OBJS = $(SRCS: .c = .o)

# Build
$(BIN): $(SRCS)
	@mkdir -p bin
	$(CC) $^ $(CFLAGS) -o $@
	@mv -f $(BIN) bin/

# Misc
clean:
	rm -f bin/$(BIN) $(OBJS)

all:
	clang

.PHONY: clean all
