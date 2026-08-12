#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <log.h>

#include "build.h"
#include "config.h"
#include "run.h"
#include "shared.h"

static char path[PM_PATH_SIZE];

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
  char executable[PM_PATH_SIZE * 2];
  char target[1024];

  if (name) {
    if (name[0] == '/')
      snprintf(executable, sizeof(executable), "%s", name);
    else
      snprintf(executable, sizeof(executable), "%s/%s", path, name);
  } else {
    read_project_target(path, target, sizeof(target));
    snprintf(executable, sizeof(executable), "%s/%s", path, target);

    // Old projects may not have a target configured yet.
    if (!is_executable(executable) &&
        find_executable(executable, sizeof(executable)) == EXIT_FAILURE)
      return EXIT_FAILURE;
  }

  if (!is_executable(executable)) {
    ERROR("The selected target is not an executable binary");
    return EXIT_FAILURE;
  }

  if (verbose)
    INFO("Running project executable");

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
  char *project;

  get_flags(argc, argv);

  if (has_flag('v')) {
    verbose = true;
  }
  if (has_flag('p')) {
    pmpath = true;
  }
  if (has_flag('b')) {
    build = true;
  }
  if (has_flag('a')) {
    args = true;
  }

  const char *flags = "pvba";
  warn_invalid_flags(strlen(flags), flags);

  project = get_flag_value(argc, argv, 'p');
  if (pmpath && !project) {
    ERROR("The -p flag needs a project name");
    return EXIT_FAILURE;
  }

  if (project) {
    if (resolve_project(project, path, sizeof(path)) == EXIT_FAILURE)
      return EXIT_FAILURE;
  } else if (current_project(path, sizeof(path)) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  // The first normal argument is the executable name.
  for (int i = 0; i < argc; i++) {
    if (i > 0 && strcmp(argv[i - 1], "-p") == 0)
      continue;

    if (strcmp(argv[i], "-a") == 0) {
      program_args = argv + i + 1;
      program_argc = argc - i - 1;

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
