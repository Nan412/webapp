# Copyright 2012, Tim Branyen (@tbranyen)
# This software may be freely distributed under the MIT license.
 
# Configure custom paths here, ideally you will only ever need to change the
# `PREFIX`.
PREFIX=$(CURDIR)
DEPS=$(PREFIX)/deps
BUILD_DIR=$(PREFIX)/bin

# Lua configuration.
LUA_DIR=$(DEPS)/lua
PLATFORM=macosx

# Luarocks configuration.
LUAROCKS_DIR=$(DEPS)/luarocks
LUAROCKS_PACKAGES=tlua ansicolors lua_cliargs

# Configure the compiler options here if they differ on your system.
CC=gcc
CCFLAGS=-Wall -o $(BUILD_DIR)/webapp

# Build everything from scratch.
all: clean lua luarocks update_luarocks build

# Build a localized version of Lua.
lua:
	@@cd $(LUA_DIR) && make INSTALL_TOP=$(LUA_DIR) $(PLATFORM) install

# Build luarocks, the Lua package manager, into the same dependency folder.
luarocks:
	@@cd $(LUAROCKS_DIR) && make clean
	@@cd $(LUAROCKS_DIR) && ./configure --prefix=$(LUAROCKS_DIR) --sysconfdir=$(LUAROCKS_DIR) --with-lua=$(LUA_DIR) --force-config
	@@cd $(LUAROCKS_DIR) make && make install

# FIXME Do not hardcode packages in here.
# Update all the luarocks packages.
update_luarocks:
ifeq ($(wildcard, $(LUAROCKS_DIR)/bin/luarocks),)
	@cd $(LUAROCKS_DIR) && ./bin/luarocks install tlua
	@cd $(LUAROCKS_DIR) && ./bin/luarocks install ansicolors
	@cd $(LUAROCKS_DIR) && ./bin/luarocks install lua_cliargs
else 
	echo "luarocks is not installed, run `make luarocks` first"
endif

build:
	@@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(PREFIX)/src/webapp.c -rpath,$(LUA_DIR)/lib -Wl,-L$(LUA_DIR)/lib,-llua

clean:
	@@rm -rf $(BUILD_DIR)
	@@rm -rf $(LUA_DIR)/include
	@@rm -rf $(LUA_DIR)/lib
	@@rm -rf $(LUA_DIR)/man
	@@rm -rf $(LUA_DIR)/src/liblua.a
	@@rm -rf $(LUA_DIR)/src/lua
	@@rm -rf $(LUA_DIR)/src/luac
	@@rm -rf $(LUAROCKS_DIR)/bin
	@@rm -rf $(LUAROCKS_DIR)/built
	@@rm -rf $(LUAROCKS_DIR)/lib
	@@rm -rf $(LUAROCKS_DIR)/share
	@@cd $(LUA_DIR) && make clean
	@@cd $(LUAROCKS_DIR) && make clean

.PHONY: bin
