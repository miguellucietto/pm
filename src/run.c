#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <log.h>

#include "build.h"
#include "run.h"
#include "shared.h"


static char *path;

//\\// ----------------------------------------------------------------//\\//
static bool verbose = false; // Print the process                  // -v
static bool pmpath = false;  // Accept the path of the pm project // -p
static bool build = false;   // Build before running              // -b
static bool args = false;    // Pass the arguments after -a       // -a
//\\// ----------------------------------------------------------------//\\//

// Check if a path is a regular file with execution permission
bool is_executable(const char *file) {
  struct stat info;
  unsigned char magic[4];

  if (stat(file, &info) == -1)
    return false;

  if (!S_ISREG(info.st_mode) ||
      !(info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
    return false;

  FILE *f = fopen(file, "rb");
  if (!f)
    return false;

  size_t bytes = fread(magic, 1, sizeof(magic), f);
  fclose(f);

  return bytes == sizeof(magic) && magic[0] == 0x7f &&
         magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F';
}

// Find the first executable in the project root
int find_executable(char *executable, size_t size) {
  DIR *dir = opendir(path);
  if (!dir) {
    ERROR("Could not open the path '%s': %s", path, strerror(errno));
    return EXIT_FAILURE;
  }

  struct dirent *file;
  while ((file = readdir(dir))) {
    if (file->d_name[0] == '.')
      continue;

    snprintf(executable, size, "%s/%s", path, file->d_name);
    if (is_executable(executable)) {
      closedir(dir);
      return EXIT_SUCCESS;
    }
  }

  closedir(dir);
  ERROR("Could not find an executable in '%s'", path);
  return EXIT_FAILURE;
}

// Verify the project and execute its binary
int run_project(const char *name, int argc, char **argv) {
  char pmpath_file[1024];
  char executable[1024];

  snprintf(pmpath_file, sizeof(pmpath_file), "%s/.pm", path);

  FILE *f = fopen(pmpath_file, "r");
  if (!f) {
    ERROR("The path '%s' is not a pm project", path);
    return EXIT_FAILURE;
  }
  fclose(f);

  if (name) {
    if (name[0] == '/')
      snprintf(executable, sizeof(executable), "%s", name);
    else
      snprintf(executable, sizeof(executable), "%s/%s", path, name);
  } else if (find_executable(executable, sizeof(executable)) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (!is_executable(executable)) {
    ERROR("The file '%s' is not an executable binary", executable);
    return EXIT_FAILURE;
  }

  if (verbose)
    INFO("Running '%s'", executable);

  // argv has enough space for the executable, its arguments and NULL.
  char *command[argc + 2];
  command[0] = executable;

  for (int i = 0; i < argc; i++) {
    command[i + 1] = argv[i];
  }
  command[argc + 1] = NULL;

  return cmd_run(command, __FILE__, __LINE__, __func__);
}

// Verify flags and execute the project
int pm_run(int argc, char **argv) {
  char *name = NULL;
  char **program_args = NULL;
  int program_argc = 0;

  get_flags(argc, argv);

  if (has_flag('v')) {
    verbose = true;
  }
  if (has_flag('p')) {
    pmpath = true;
    path = argv[argc - 1];
  } else {
    path = ".";
  }
  if (has_flag('b')) {
    build = true;
  }
  if (has_flag('a')) {
    args = true;
  }

  const char *flags = "pvba";
  warn_invalid_flags(strlen(flags), flags);

  if (pmpath && is_flag(path)) {
    ERROR("The -p flag needs a project path");
    return EXIT_FAILURE;
  }

  // The first normal argument is the executable name.
  for (int i = 0; i < argc; i++) {
    if (pmpath && i == argc - 1)
      continue;

    if (strcmp(argv[i], "-a") == 0) {
      program_args = argv + i + 1;
      program_argc = argc - i - 1;

      // With -p, both "-p" and its final path belong to pm.
      if (pmpath)
        program_argc -= 2;
      break;
    }

    if (!is_flag(argv[i]) && !name)
      name = argv[i];
  }

  if (build && build_project(path) == EXIT_FAILURE)
    return EXIT_FAILURE;

  if (!args) {
    program_args = NULL;
    program_argc = 0;
  }

  return run_project(name, program_argc, program_args);
}
