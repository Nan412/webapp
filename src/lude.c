#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(JIT)
  #include "../include/luajit-2.0/lua.h"
  #include "../include/luajit-2.0/luaconf.h"
  #include "../include/luajit-2.0/lauxlib.h"
  #include "../include/luajit-2.0/lualib.h"
#elif defined(NATIVE)
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
  lua_State *L = lua_open();

  // Open lua libraries
  luaL_openlibs(L);

  // Open file buffer
  const char* buff;
  if(argc > 1) {
    buff = ReadFile(argv[1]);
  }
  else {
    // Maybe default to REPL here
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
    if (error) {
      fprintf(stderr, "%s", lua_tostring(L, -1));

      // Pop error from the stack
      lua_pop(L, 1);
    }
  }

  lua_close(L);
  return 0;
}
