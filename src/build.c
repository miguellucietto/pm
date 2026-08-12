#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "build.h"
#include "config.h"
#include "shared.h"

static char path[PM_PATH_SIZE];

//\\// ----------------------------------------------------------------//\\//
static bool pmpath = false; // Accept the path of the pm project // -p
static bool verbose = false; // Print the process                // -v
//\\// ----------------------------------------------------------------//\\//

// Verify the project and call its Makefile
int build_project(const char *root) {
  char makefile[PM_PATH_SIZE];
  snprintf(makefile, sizeof(makefile), "%s/Makefile", root);

  FILE *f = fopen(makefile, "r");
  if (!f) {
    ERROR("Could not find the Makefile in '%s'", root);
    return EXIT_FAILURE;
  }
  fclose(f);

  if (verbose)
    INFO("Building project in '%s'", root);

  return CMD("make", "-C", (char *)root, "all");
}

// Verify flags and build the project
int pm_build(int argc, char **argv) {
  char *project;

  get_flags(argc, argv);

  pmpath = has_flag('p');
  if (has_flag('v')) {
    verbose = true;
  }

  const char *flags = "pv";
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

  return build_project(path);
}
