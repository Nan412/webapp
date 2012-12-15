# Copyright 2012, Tim Branyen (@tbranyen)
# This software may be freely distributed under the MIT license.
 
# Configure custom paths here, ideally you will only ever need to change the
# `PREFIX`.
PREFIX=$(CURDIR)
DEPS=$(PREFIX)/deps
BUILD_DIR=$(PREFIX)/bin

# Luarocks configuration.
LUAROCKS_DIR=$(DEPS)/luarocks
LUAROCKS_PACKAGES=tlua ansicolors cliargs

# Configure the compiler options here if they differ on your system.
CC=gcc
CCFLAGS=-Wall -o
PLATFORM=posix

# Default command.
all: clean build

# Update the build.
update: clean build

lua:
	cd deps/lua
	make $(PLATFORM)
	make install

# Build luarocks, the Lua package manager, into the same dependency folder.
luarocks:
	@@cd $(LUAROCKS_DIR) && make clean
	@@cd $(LUAROCKS_DIR) && ./configure --prefix=$(LUAROCKS_DIR) --sysconfdir=$(LUAROCKS_DIR) --force-config
	@@cd $(LUAROCKS_DIR) make && make install

update_luarocks:
ifeq ($(wildcard, $(LUAROCKS_DIR)/bin/luarocks),)
	@cd $(LUAROCKS_DIR) && ./bin/luarocks install $(LUAROCKS_PACKAGES)
else 
	echo "luarocks is not installed, run `make luarocks` first"
endif

build:
	@@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c -llua

clean:
	@@rm -rf $(BUILD_DIR)
	(cd $(LUAROCKS_DIR) && make clean)


.PHONY: bin
