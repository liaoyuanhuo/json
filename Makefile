# Install
BIN = json

# Compiler
CC = gcc
DCC = clang

#Flags
DFLAGS = -g -Wall -Wextra -Werror -Wformat=2 -Wunreachable-code
DFLAGS += -fstack-protector-strong -Winline -Wshadow -Wwrite-strings -fstrict-aliasing
DFLAGS += -Wstrict-prototypes -Wold-style-definition -Wconversion
DFLAGS += -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
DFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wmissing-prototypes -Wconversion
DFLAGS += -Wswitch-default -Wundef -Wno-unused -Wstrict-overflow=5 -Wsign-conversion
DFLAGS += -Winit-self -Wstrict-aliasing
CFLAGS = -O3 -fno-gcse

.PHONY: release
release: $(BIN)

.PHONY: debug
debug: CFLAGS = $(DFLAGS)
debug: $(BIN)

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
	release

.PHONY: clean all
