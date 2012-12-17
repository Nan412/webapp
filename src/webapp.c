// Standard libraries.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

// Include Lua.
#include "../deps/lua/src/lua.h"
#include "../deps/lua/src/luaconf.h"
#include "../deps/lua/src/lauxlib.h"
#include "../deps/lua/src/lualib.h"

// Windows support.
#if _WIN32 || _WIN64
#include <Windows.h>
#define snprintf _snprintf
#endif

// Awesome cross platform function to find the execution path.  Adapted from
// Hiperion's answer in StackOverflow.
//
// http://stackoverflow.com/questions/606041/how-do-i-get-the-path-of-a-process
// -in-unix-linux
char* find_execution_path(char* path, size_t dest_len, char* argv0) {
  char* baseName = NULL;
  char* systemPath = NULL;
  char* candidateDir = NULL;
  char buf[FILENAME_MAX];

  // The easiest case: Linux.  If it's not here, there is no guarentee.
  if (readlink("/proc/self/exe", path, dest_len) != -1) {
    dirname(path);
    strcat(path, "/");

		// Remove the trailing folder and bin.
		path[strlen(path)-4] = '\0';

    return path;
  }

  /* check if we have something like execve("foobar", NULL, NULL) */
  if (argv0 == NULL) {
    /* we surrender and give current path instead */
    if (getcwd(path, dest_len) == NULL) {
      return NULL;
    }

    strcat(path, "/");

    return path;
  }

  /* argv[0] */
  /* if dest_len < PATH_MAX may cause buffer overflow */
  if ((realpath (argv0, path)) && (!access (path, F_OK))) {
    dirname(path);
    strcat(path, "/");

    return path;
  }

  /* Current path */
  baseName = basename(argv0);

  if (getcwd (path, dest_len - strlen (baseName) - 1) == NULL) {
    return NULL;
  }

  strcat(path, "/");
  strcat(path, baseName);

  if (access(path, F_OK) == 0) {
    dirname(path);
    strcat(path, "/");

    return path;
  }

  /* Try the PATH. */
  systemPath = getenv ("PATH");

  if (systemPath != NULL) {
    dest_len--;
    systemPath = strdup(systemPath);

    for (candidateDir = strtok(systemPath, ":"); candidateDir != NULL;
      candidateDir = strtok(NULL, ":")) {

      strncpy(path, candidateDir, dest_len);
      strncat(path, "/", dest_len);
      strncat(path, baseName, dest_len);

      if (access(path, F_OK) == 0) {
        free(systemPath);
        dirname(path);
        strcat(path, "/");

        // Strip off the trailing `/`.
        path[strlen(path)-1] = '\0';

        // Attempt checking if this is a symbolic link.
        readlink(path, path, sizeof(buf)-1);

        // Remove the trailing folder and bin.
        path[strlen(path)-10] = '\0';

        // Hard path.
        return path;
      }
    }

    free(systemPath);
    dest_len++;
  }

  /* again someone has use execve: we dont knowe the executable name; we
   * surrender and give instead current path */
  if (getcwd (path, dest_len - 1) == NULL) {
    return NULL;
  }

  strcat(path, "/");

  return path;
}

// ~ Read Lua configuration file. ~
int load_lua_cli(char* root, char** buffer) {
  FILE* file = fopen(strncat(root, "lib/cli.lua", FILENAME_MAX), "rb");
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
  //char* currentPath = ".";
  // FIXME Weird segmentation fault when running the following code.
  char currentPath[FILENAME_MAX];
  // Used to store the contents of the CLI script.
  char* buff;
  // Used for iteration and tracking errors.
  int i, error;
  // Used for storing paths that are convenience for the Lua being written.
  char *paths, *absolutePaths, *cpaths, *absoluteCPaths;
  // Maintain the Lua runtime state.
  lua_State* L;

  // Set the current path.
  find_execution_path(currentPath, sizeof(currentPath), argv[0]);

  // The template string for Lua module paths.
  paths =
    "%s/?.lua;%s/deps/luarocks/share/lua/5.2/?.lua;%s/deps/luarocks/share/lua/"
    "5.2/?/init.lua;%s/deps/luarocks/lib/lua/5.2/?.lua;%s/deps/luarocks/lib/lu"
    "a/5.2/?/init.lua";
  // Allocate the memory to store the completed absolute paths.
  absolutePaths = malloc(snprintf(NULL, 0, paths, currentPath,
    currentPath, currentPath, currentPath, currentPath) + 1);
  // Normalize paths to their absolute form.
  sprintf(absolutePaths, paths, currentPath, currentPath, currentPath,
    currentPath, currentPath);
  // The template string for Lua module paths.
  cpaths =
    "%s/?.so;%s/deps/luarocks/lib/lua/5.2/?.so;%s/deps/luarocks/lib/lua/5.2/lo"
    "adall.so;%s/deps/luafilesystem/src/?.so";
  // Allocate the memory to store the completed absolute cpaths.
  absoluteCPaths = malloc(snprintf(NULL, 0, cpaths, currentPath,
    currentPath, currentPath) + 1);
  // Normalize cpaths to their absolute form.
  sprintf(absoluteCPaths, cpaths, currentPath, currentPath, currentPath);

  // Open lua.
  L = luaL_newstate();

  // Open lua libraries.
  luaL_openlibs(L);

  // Get the package table.
  lua_getglobal(L, "package");

  // This gets the `path` property from the package table.  At the top of the 
  // stack, (-1).
  lua_getfield(L, -1, "path");

  // Remove the String from the stack.
  lua_pop(L, 1);

  // Push the new String onto the stack.
  lua_pushstring(L, absolutePaths);

  // Set the field "path" in table at -2 with value at top of stack.
  lua_setfield(L, -2, "path");

  // Remove the path.
  lua_pop(L, 1);

  // Add the cpath.
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "cpath");
  lua_pop(L, 1);
  lua_pushstring(L, absoluteCPaths);
  lua_setfield(L, -2, "cpath");
  lua_pop(L, 1);
  
  // Open file buffer.
  error = load_lua_cli(currentPath, &buff);

  // File read error.
  if (error < 0) {
    fprintf(stderr, "%s\n", "Unable read or parse the main CLI file.");
    return 1;
  }

  // Load the buffer.
  error = luaL_loadstring(L, buff);

  if (error) {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));

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
    fprintf(stderr, "%s\n", lua_tostring(L, -1));

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
