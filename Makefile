# Package version number
PACKAGE_VERSION = 1.0

# Compiler options
CFLAGS = -g -Wall -Wextra -Werror -std=c17 -O3
LDLIBS = -lpthread
CC = gcc
AR = ar rcs
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

# Installation prefix
PREFIX = /usr/local

# OS Detection
OSFLAG :=
ifeq ($(OS), Windows_NT)
OSFLAG += -D WIN32
VALGRIND = ''
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG += -D LINUX -D _GNU_SOURCE
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG += -D MACOS
		VALGRIND =
	endif
endif

# Build a list of targets by listing the example C files and
# removing the .c extension
EXAMPLES = $(patsubst examples/%.c,%,$(wildcard examples/*.c))

# Installation prefix
PREFIX = /usr/local

all: libvlogger

prereq:
	mkdir -p bin/examples lib

libvlogger: clean prereq bin/vlogger.o
	$(AR) lib/libvlogger.a bin/vlogger.o

bin/vlogger.o: vlogger.c
	$(CC) $(CFLAGS) -c -o $@ $(OSFLAG) $<

prereq/tests:
	$(eval CFLAGS += -DTest_operations)

tests: clean prereq/tests bin/vlogger.o
	$(CC) $(CFLAGS) -o bin/vlogger $(OSFLAG) $(LDLIBS) bin/vlogger.o
	$(VALGRIND) bin/vlogger

# Install/uninstall targets
install: libvlogger
	$(eval INSTALL_LIB_DIR = $(PREFIX)/lib)
	$(eval INSTALL_INC_DIR = $(PREFIX)/include)
	if test ! -d "$(INSTALL_LIB_DIR)"; then mkdir -vp "$(INSTALL_LIB_DIR)"; fi \
	&& if test ! -d "$(INSTALL_INC_DIR)"; then mkdir -vp "$(INSTALL_INC_DIR)"; fi \
	&& cp lib/libvlogger.a "$(INSTALL_LIB_DIR)/libvlogger-$(PACKAGE_VERSION).a" \
	&& cp vlogger.h "$(INSTALL_INC_DIR)/vlogger.h" \
	&& cd $(INSTALL_LIB_DIR) \
	&& ln -s libvlogger-$(PACKAGE_VERSION).a libvlogger.a

uninstall:
	$(eval INSTALL_LIB_DIR = $(PREFIX)/lib)
	$(eval INSTALL_INC_DIR = $(PREFIX)/include)
	rm -fv "$(INSTALL_INC_DIR)/"vlogger*.* \
	&& rm -fv "$(INSTALL_LIB_DIR)/"libvlogger*.*

# Wildcard compilation for examples
examples: libvlogger $(EXAMPLES)

$(EXAMPLES): %: examples/%.c
	$(CC) $(CFLAGS) $< $(OSFLAG) -Llib -lvlogger $(LDLIBS) -o bin/examples/$@


clean:
	rm -vrf bin/** lib/**
