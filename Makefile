# Install
BIN = json

# Compiler
CC = gcc

#Flags
CFLAGS = -g -Wall -Wextra -Wformat=2 -Wunreachable-code
CFLAGS += -fstack-protector-strong -Winline -Wshadow -Wwrite-strings -fstrict-aliasing
CFLAGS += -Wstrict-prototypes -Wold-style-definition -Wconversion
CFLAGS += -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wmissing-prototypes -Wconversion
CFLAGS += -Wswitch-default -Wundef -Wno-unused -Wstrict-overflow=5 -Wsign-conversion
CFLAGS += -Winit-self -Wstrict-aliasing -fsanitize=address -fno-omit-frame-pointer

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
