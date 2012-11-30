#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// This prototype uses the luajit headers instead of standard Lua.
#include "../include/luajit-2.0/lua.h"
#include "../include/luajit-2.0/luaconf.h"
#include "../include/luajit-2.0/lauxlib.h"
#include "../include/luajit-2.0/lualib.h"

// ~ Read files. ~
char* readFile(char* path) {
  FILE* file;
  char* buffer;
  unsigned long fileLen;

  fopen_s(&file, path, "rb");

  if(!file) {
    fprintf(stderr, "Unable to open file %s\n", path);
    return "";
  }

  fseek(file, 0, SEEK_END);
  fileLen = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = (char*)malloc(fileLen+1);
  if(!file) {
    fprintf(stderr, "Out of memory or something\n");
    fclose(file);

    return "";
  }

  fread(buffer, fileLen, 1, file);
  fclose(file);

  return buffer;
}

// Kick off the tool!
int main (int argc, char** argv) {
  // Used to store the contents of the CLI script.
  const char* buff;
  // Used for iteration and tracking errors.
  int i, error;

  // Open lua.
  lua_State* L = luaL_newstate();

  // Open lua libraries.
  luaL_openlibs(L);

  // Get the package table.
  lua_getglobal(L, "package");

  // This gets the `path` property from the package table.  At the top of the stack, (-1).
  lua_getfield(L, -1, "path");

  // Remove the String from the stack.
  lua_pop(L, 1);

  // Push the new String onto the stack.
  lua_pushstring(L, "./?.lua;./deps/luarocks/share/lua/5.1/?.lua;./deps/luarocks/share/lua/5.1/?/init.lua;./deps/luarocks/lib/lua/5.1/?.lua;./deps/luarocks/lib/lua/5.1/?/init.lua");

  // Set the field "path" in table at -2 with value at top of stack.
  lua_setfield(L, -2, "path");

  // Remove the path.
  lua_pop(L, 1);

  // Add the cpath.
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "cpath");
  lua_pop(L, 1);
  lua_pushstring(L, "./?.so;./deps/luarocks/lib/lua/5.1/?.so;./deps/luarocks/lib/lua/5.1/loadall.so");
  lua_setfield(L, -2, "cpath");
  lua_pop(L, 1);
  
  // Open file buffer.
  buff = readFile("lib/cli.lua");

  // Load the buffer.
  error = luaL_loadstring(L, buff);

  if (error) {
    fprintf(stderr, "%s", lua_tostring(L, -1));

    // Pop error from the stack.
    lua_pop(L, 1);

    // Err code.
    return 1;
  }

  // Prepare arguments to be passed into Lua-land.
  lua_createtable (L, argc, 0);
  for (i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
  }

  // Map the table to the internal global `arg`.
  lua_setglobal(L, "arg");

  // Execute Lua.
  error = lua_pcall(L, 0, 0, 0);

  // Report all errors.
  if (error) {
    fprintf(stderr, "%s", lua_tostring(L, -1));

    // Pop error from the stack
    lua_pop(L, 1);

    // Err code.
    return 1;
  }

  // End the run.
  lua_close(L);

  // Normal exit code.
  return 0;
}
