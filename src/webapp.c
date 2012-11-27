#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(NATIVE)
  #include "../include/lua.h"
  #include "../include/luaconf.h"
  #include "../include/lauxlib.h"
  #include "../include/lualib.h"
#endif

char* ReadFile(char* path) {
  FILE* file;
  char* buffer;
  unsigned long fileLen;

  file = fopen(path, "rb");
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


int main (int argc, char** argv) {
  // Open lua
  lua_State* L = luaL_newstate();

  // Open lua libraries
  luaL_openlibs(L);

  // Get the package table.
  lua_getglobal(L, "package");
  // This gets the `path` property from the package table.  At the top of the stack, (-1).
  lua_getfield(L, -1, "path");

  // Remove the String from the stack.
  lua_pop(L, 1);

  // Push the new String onto the stack.
  lua_pushstring(L, "./?.lua;./deps/luarocks/share/lua/5.1/?.lua;./deps/luarocks/share/lua/5.1/?/init.lua;./deps/luarocks/lib/lua/5.1/?.lua;./deps/luarocks/lib/lua/5.1/?/init.lua");

  // set the field "path" in table at -2 with value at top of stack
  lua_setfield(L, -2, "path");
  lua_pop(L, 1);

  // Add the cpath.
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "cpath");
  lua_pop(L, 1);
  lua_pushstring(L, "./?.so;./deps/luarocks/lib/lua/5.1/?.so;./deps/luarocks/lib/lua/5.1/loadall.so");
  lua_setfield(L, -2, "cpath");

  lua_pop(L, 1);

  const char* buff;

  // Open file buffer
  buff = ReadFile("lib/cli.lua");

  // Load the buffer.
  int error = luaL_loadstring(L, buff);
  int i;

  if (error) {
    fprintf(stderr, "%s", lua_tostring(L, -1));

    // Pop error from the stack
    lua_pop(L, 1);

    return 1;
  }

  lua_createtable (L, argc, 0);
  for (i = 0; i < argc; i++) {
    lua_pushstring (L, argv[i]);
    lua_rawseti(L, -2, i);
  }
  lua_setglobal(L, "arg");

  error = lua_pcall(L, 0, 0, 0);

  if (error) {
    fprintf(stderr, "%s", lua_tostring(L, -1));

    // Pop error from the stack
    lua_pop(L, 1);

    return 1;
  }

  lua_close(L);

  return 0;
}
