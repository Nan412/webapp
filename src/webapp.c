#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Include the Lua headers.
#include "../deps/lua/include/lua.h"
#include "../deps/lua/include/luaconf.h"
#include "../deps/lua/include/lauxlib.h"
#include "../deps/lua/include/lualib.h"

// ~ Read Lua configuration file. ~
int load_lua_cli(char** buffer) {
  FILE* file = fopen("lib/cli.lua", "rb");
  int length;

  // FIXME Visual Studio complains that this is safer, is it?
  //fopen_s(&file, path, "rb");

  // Ensure the file exists.
  if (file == NULL) {
    // Unable to locate file.
    return -1;
  }

  // Read till end of file.
  fseek(file, 0, SEEK_END);

  // Determine length.
  length = ftell(file);

  // Reset seek position.
  fseek(file, 0, SEEK_SET);

  // Assign and allocate to the buffer.
  *buffer = (char*)malloc(length+1);

  // Determine read failure.
  if (length != fread(*buffer, sizeof(char), length, file)) {
    // Close the file handle.
    fclose(file);

    // Empty filled buffer.
    free(*buffer);

    // Unable to read file.
    return -2;
  }

  // Close the file handle.
  fclose(file);

  (*buffer)[length] = 0;

  return 0;
}

// Kick off the tool!
int main (int argc, char** argv) {
  // Used to store the contents of the CLI script.
  char* buff;
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
  lua_pushstring(L, "./?.lua;./deps/luarocks/share/lua/5.2/?.lua;./deps/luarocks/share/lua/5.2/?/init.lua;./deps/luarocks/lib/lua/5.2/?.lua;./deps/luarocks/lib/lua/5.2/?/init.lua");

  // Set the field "path" in table at -2 with value at top of stack.
  lua_setfield(L, -2, "path");

  // Remove the path.
  lua_pop(L, 1);

  // Add the cpath.
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "cpath");
  lua_pop(L, 1);
  lua_pushstring(L, "./?.so;./deps/luarocks/lib/lua/5.2/?.so;./deps/luarocks/lib/lua/5.2/loadall.so");
  lua_setfield(L, -2, "cpath");
  lua_pop(L, 1);
  
  // Open file buffer.
  error = load_lua_cli(&buff);

  // File read error.
  if (error < 0) {
    fprintf(stderr, "%s", "Unable read or parse the main CLI file.");
  }

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
