# Copyright 2012, Tim Branyen (@tbranyen)
# This software may be freely distributed under the MIT license.  
# Host OS detection, removes need for checking.
UNAME= $(shell uname)
 
# Configure custom paths here, ideally you will only ever need to change the
# `PREFIX`.
PREFIX=$(CURDIR)
DEPS=$(PREFIX)/deps
BUILD_DIR=$(PREFIX)/bin

# Lua configuration.
LUA_DIR=$(DEPS)/lua
# Defaults to posix, override with custom.
PLATFORM=posix

# MoonBox configuration.
MOONBOX_DIR=$(DEPS)/moonbox
MOONBOX_HIDDEN_DIR=$(PREFIX)/.moonbox

# LuaFileSystem configuration.
LUAFILESYSTEM_DIR=$(DEPS)/luafilesystem

# Configure the compiler options here if they differ on your system.
CC=gcc
CC_FLAGS=-Wall
LD_FLAGS=-Wl,-rpath,$(LUA_DIR)/lib
LINUX_FLAGS=-I$(LUA_DIR)/include -L$(LUA_DIR)/lib -llua -lm -ldl -lc

# Build `webapp` from source.
all: update_submodules lua luafilesystem build

test:
	@@busted -c test/**/*.lua

update_submodules:
	git submodule update --init --recursive

# Build a localized version of Lua.
lua:
ifeq "$(UNAME)" "Linux"
	@@cd $(LUA_DIR) && make INSTALL_TOP=$(LUA_DIR) linux install
endif
ifeq "$(UNAME)" "Darwin"
	@@cd $(LUA_DIR) && make INSTALL_TOP=$(LUA_DIR) macosx install
else
	@@cd $(LUA_DIR) && make INSTALL_TOP=$(LUA_DIR) $(PLATFORM) install
endif

moonbox:
	@@cd $(MOONBOX_DIR) && ./configure --prefix=$(MOONBOX_DIR)
	@@cd $(MOONBOX_DIR) && make install

# FIXME Should this instead go into the Lua directory or someplace else?
luafilesystem:
	@@cd $(LUAFILESYSTEM_DIR) && make

# Install all the Lua dependencies.
install_deps:
	@@$(MOONBOX_DIR)/bin/moonbox env enter
	@@$(MOONBOX_DIR)/bin/moonbox install

# Actually build the application.
build: cleanbuild
	@@mkdir -p $(BUILD_DIR)
	@@$(CC) $(CC_FLAGS) $(LD_FLAGS) -o $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c $(LINUX_FLAGS)

# Do a full removal of the compiled parts.  Typically you will never need to
# run this.
clean:
	@@$(MOONBOX_DIR)/bin/moonbox env leave
	@@rm -rf $(BUILD_DIR)
	@@rm -rf $(LUA_DIR)/include
	@@rm -rf $(LUA_DIR)/lib
	@@rm -rf $(LUA_DIR)/man
	@@rm -rf $(LUA_DIR)/src/liblua.a
	@@rm -rf $(LUA_DIR)/src/lua
	@@rm -rf $(LUA_DIR)/src/luac
	@@rm -rf $(MOONBOX_HIDDEN_DIR)
	@@cd $(LUA_DIR) && make clean

# Only remove the build files.
cleanbuild:
	@@rm -rf $(BUILD_DIR)

link:
ifeq ($(shell whoami), root)
	@@ln -sf $(BUILD_DIR)/webapp /usr/local/bin/webapp
else
	@@echo "You need to run this command as root, perhaps sudo."
endif

unlink:
ifeq ($(shell whoami), root)
	@@unlink /usr/local/bin/webapp
else
	@@echo "You need to run this command as root, perhaps sudo."
endif

.PHONY: bin test
