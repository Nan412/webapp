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
	$(CC) $(CCFLAGS) $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c -llua

clean:
	@@rm -rf $(BUILD_DIR)

.PHONY: bin
