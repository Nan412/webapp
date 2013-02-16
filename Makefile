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

# Luarocks configuration.
LUAROCKS_DIR=$(DEPS)/luarocks
LUAROCKS_PACKAGES=tlua ansicolors lua_cliargs

# LuaFileSystem configuration.
LUAFILESYSTEM_DIR=$(DEPS)/luafilesystem

# Configure the compiler options here if they differ on your system.
CC=gcc
CC_FLAGS=-Wall
LD_FLAGS=-Wl,-rpath,$(LUA_DIR)/lib
LINUX_FLAGS=-I$(LUA_DIR)/include -L$(LUA_DIR)/lib -llua -lm -ldl -lc

# Build everything from scratch.
all: clean update_submodules lua luarocks luafilesystem update_luarocks build

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

# Build luarocks, the Lua package manager, into the same dependency folder.
luarocks:
	@@cd $(LUAROCKS_DIR) && make clean
	@@cd $(LUAROCKS_DIR) && ./configure --prefix=$(LUAROCKS_DIR) --sysconfdir=$(LUAROCKS_DIR) --with-lua=$(LUA_DIR) --force-config
	@@cd $(LUAROCKS_DIR) make && make install

# FIXME Should this instead go into the Lua directory or someplace else?
luafilesystem:
	@@$(CC) $(CC_FLAGS) $(LD_FLAGS) -shared -o $(LUAFILESYSTEM_DIR)/src/lfs.so $(LUAFILESYSTEM_DIR)/src/lfs.c $(LINUX_FLAGS)

# FIXME Do not hardcode packages in here.
# Update all the luarocks packages.
update_luarocks:
ifeq ($(wildcard, $(LUAROCKS_DIR)/bin/luarocks),)
	$(LUAROCKS_DIR)/bin/luarocks install tlua
	$(LUAROCKS_DIR)/bin/luarocks install ansicolors
	$(LUAROCKS_DIR)/bin/luarocks install lua_cliargs
	$(LUAROCKS_DIR)/bin/luarocks install json4lua
else 
	echo "luarocks is not installed, run `make luarocks` first"
endif

# Actually build the application.
build: cleanbuild
	@@mkdir -p $(BUILD_DIR)
	@@$(CC) $(CC_FLAGS) $(LD_FLAGS) -o $(BUILD_DIR)/webapp $(PREFIX)/src/webapp.c $(LINUX_FLAGS)

# Do a full removal of the compiled parts.  Typically you will never need to
# run this.
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
	@@rm -rf $(LUAROCKS_DIR)/config.lua
	@@cd $(LUA_DIR) && make clean
	@@cd $(LUAROCKS_DIR) && make clean
	@@touch $(LUAROCKS_DIR)/config.unix

# Only remove the build files.
cleanbuild:
	@@rm -rf $(BUILD_DIR)

link:
ifeq ($(shell whoami), root)
	@@ln -s $(BUILD_DIR)/webapp /usr/local/bin/webapp
else
	@@echo "You need to run this command as root, perhaps sudo."
endif

unlink:
ifeq ($(shell whoami), root)
	@@unlink /usr/local/bin/webapp
else
	@@echo "You need to run this command as root, perhaps sudo."
endif

.PHONY: bin
