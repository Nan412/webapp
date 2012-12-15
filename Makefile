# Copyright 2012, Tim Branyen (@tbranyen)
# This software may be freely distributed under the MIT license.
 
# Configure custom paths here, ideally you will only ever need to change the
# `PREFIX`.
PREFIX=.
DEPS=$(PREFIX)/deps
BUILD_DIR=$(PREFIX)/bin

# Configure the compiler options here if they differ on your system.
CC=gcc
CCFLAGS=-Wall -o
PLATFORM=posix

# Default command.
all: clean build

# Update the build.
update: clean build

# Build luarocks, the Lua package manager, into the same dependency folder.
luarocks:
	# Set the luarocks path.
	LUAROCKS_DIR=$(DEPS)/luarocks
	# Build locally.
	cd $(LUAROCKS_DIR)
	./configure --prefix=$(LUAROCKS_DIR) --sysconfdir=$(LUAROCKS_DIR) --force-config
	make
	make install

lua:
	cd deps/lua
	make $(PLATFORM)
	make install

build:
	@@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c -llua

clean:
	@@rm -rf $(BUILD_DIR)
	# Clean luarocks.
	cd $(LUAROCKS_DIR)
	make clean


.PHONY: bin
