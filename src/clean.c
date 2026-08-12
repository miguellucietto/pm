#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "clean.h"
#include "config.h"
#include "shared.h"

static bool verbose = false; // Print the process // -v

/* Ask the project's Makefile to remove generated files. */
int clean_project(const char *path) {
  char makefile[PM_PATH_SIZE];

  snprintf(makefile, sizeof(makefile), "%s/Makefile", path);
  FILE *f = fopen(makefile, "r");
  if (!f) {
    ERROR("Could not find the Makefile in '%s'", path);
    return EXIT_FAILURE;
  }
  fclose(f);

  if (verbose)
    INFO("Cleaning project in '%s'", path);

  return CMD("make", "-C", (char *)path, "clean");
}

/* Parse `pm clean [-v] [-p project]`. */
int pm_clean(int argc, char **argv) {
  char path[PM_PATH_SIZE];
  char *project = NULL;

  get_flags(argc, argv);
  verbose = has_flag('v');

  const char *flags = "pv";
  warn_invalid_flags(strlen(flags), flags);

  project = get_flag_value(argc, argv, 'p');
  if (has_flag('p') && !project) {
    ERROR("The -p flag needs a project name");
    return EXIT_FAILURE;
  }

  if (project) {
    if (resolve_project(project, path, sizeof(path)) == EXIT_FAILURE)
      return EXIT_FAILURE;
  } else if (current_project(path, sizeof(path)) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  return clean_project(path);
}
