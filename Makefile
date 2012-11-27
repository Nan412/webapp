# Variables.
PREFIX=.
BUILD_DIR=$(PREFIX)/bin
CC=gcc
CCFLAGS=-Wall -o

# Default command.
all: update

# Update the build.
update: clean build

build:
	@@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c -llua -lm -ldl -DNATIVE

jit:
	@@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c -L$(PREFIX)/lib -lluajit-5.1 -Wl,-rpath $(PREFIX)/lib -DJIT

clean:
	@@rm -rf $(BUILD_DIR)

.PHONY: bin
