// Standard libraries.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Include Lua.
#include "../deps/lua/src/lua.h"
#include "../deps/lua/src/luaconf.h"
#include "../deps/lua/src/lauxlib.h"
#include "../deps/lua/src/lualib.h"

#if _WIN32 || _WIN64
#include <Windows.h>
#define snprintf _snprintf

#elif __APPLE__ || __linux
#include <unistd.h>
#include <libgen.h>

#elif __unix || __posix
#endif

// http://stackoverflow.com/questions/606041/how-do-i-get-the-path-of-a-process
// -in-unix-linux
char* find_execution_path(char* path, char* argv0) {
  char* baseName = NULL;
  char* systemPath = NULL;
  char* candidateDir = NULL;
  size_t dest_len = FILENAME_MAX;
  char buf[dest_len];

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

    // Remove the trailing folder and bin.
    path[strlen(path)] = '\0';

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
  // FIXME Weird segmentation fault when running the following code.
  char currentPath[FILENAME_MAX];
  // Used to store the contents of the CLI script.
  char* buff;
  // Used for iteration and tracking errors.
  int i, error;
  // Used for storing paths as a convenience for the Lua being executed.
  char *paths;
  // Maintain the Lua runtime state.
  lua_State* L;

  // Set the current path.
  find_execution_path(currentPath, argv[0]);

  // The template string for Lua module paths.
  paths =
    "./?.so;"
    "./.moonbox/lib/lua/5.2/?.so;"
    "./?.lua;"
    "./.moonbox/share/lua/5.2/?/init.lua;"
    "./.moonbox/share/lua/5.2/?.lua";

  fprintf(stdout, "%s\n", paths);

  // Test this out.
  setenv("LUA_PATH", paths, 1);
  setenv("LUA_CPATH", paths, 1);

  // Open lua.
  L = luaL_newstate();

  // Open lua libraries.
  luaL_openlibs(L);

  // Open file buffer.
  error = load_lua_cli(currentPath, &buff);

  // File read error.
  if (error < 0) {
    fprintf(stderr, "%s\n", "Unable read or parse the main CLI file.");
    fprintf(stderr, "%s\n", currentPath);
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
