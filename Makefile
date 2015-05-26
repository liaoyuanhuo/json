# Install
BIN = json

# Compiler
CC = clang

# ANSI C
CFLAGS = -std=c89 -pedantic -g -Wall -Wextra -DJSON_USE_ANSI_C

# GCC / CLANG
# CFLAGS = -g -Wall -Wextra

.PHONY: gcc
gcc: CC = gcc
gcc: CFLAGS += -fno-gcse -fno-crossjumping
gcc: $(BIN)

.PHONY: clang
clang: CC = clang
clang: $(BIN)

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
