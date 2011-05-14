#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/lua.h"
#include "../include/luaconf.h"
#include "../include/lauxlib.h"
#include "../include/lualib.h"

char* ReadFile(char* path) {
  FILE* file;
  char* buffer;
  unsigned long fileLen;

  file = fopen(path, "rb");
  if(!file) {
    fprintf(stderr, "Unable to open file %s", path);
    return "";
  }

  fseek(file, 0, SEEK_END);
  fileLen = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = (char*)malloc(fileLen+1);
  if(!file) {
    fprintf(stderr, "Out of memory");
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
    fprintf(stderr, "No file provided");
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
