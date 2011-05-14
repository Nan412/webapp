#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/luajit-2.0/lua.h"
#include "../../include/luajit-2.0/luaconf.h"
#include "../../include/luajit-2.0/lauxlib.h"
#include "../../include/luajit-2.0/lualib.h"

#include "../../vendor/libuv/uv.h"

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
  lua_State *L = lua_open();

  // Open lua libraries
  luaL_openlibs(L);

  const char* buff;
  if(argc > 1) {
    buff = ReadFile(argv[1]);
  }
  else {
    fprintf(stderr, "No file provided\n");
    return 0;
  }

  int error = luaL_loadstring(L, buff);
  if (error) {
    fprintf(stderr, "%s", lua_tostring(L, -1));

    // Pop error from the stack
    lua_pop(L, 1);
  }
  else {
    error = lua_pcall(L, 0, 0, 0);
  }

  lua_close(L);
  return 0;
}
