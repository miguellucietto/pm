#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <log.h>

#include "config.h"
#include "goto.h"
#include "shared.h"

/* Return whether path still points to a directory. */
static int directory_exists(const char *path) {
  struct stat info;
  return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

/* Resolve a project without producing a generic config error. */
static int find_project(const char *project, char *path, size_t size) {
  char registry[PM_PATH_SIZE], line[PM_PATH_SIZE * 2];
  char name[256], value[PM_PATH_SIZE];

  if (get_projects_file_path(registry, sizeof(registry)) == EXIT_FAILURE)
    return EXIT_FAILURE;

  FILE *f = fopen(registry, "r");
  if (!f)
    return EXIT_FAILURE;

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%255[^=]=%1023[^\n]", name, value) == 2 &&
        strcmp(name, project) == 0) {
      snprintf(path, size, "%s", value);
      fclose(f);
      return EXIT_SUCCESS;
    }
  }

  fclose(f);
  WARN("Project '%s' is not registered", project);
  return EXIT_FAILURE;
}

/*
 * Print a registered project's path for use by the caller's shell.
 *
 * A child process cannot change its parent shell directory. The shell helper
 * documented in README.md captures this output and performs the actual cd.
 */
int pm_goto(int argc, char **argv) {
  char path[PM_PATH_SIZE];

  if (argc != 1 || is_flag(argv[0])) {
    WARN("Usage: pm goto <project>");
    return EXIT_FAILURE;
  }

  if (find_project(argv[0], path, sizeof(path)) == EXIT_FAILURE)
    return EXIT_FAILURE;

  if (!directory_exists(path)) {
    WARN("Project '%s' is missing from '%s'", argv[0], path);
    return EXIT_FAILURE;
  }

  puts(path);
  return EXIT_SUCCESS;
}
