#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "build.h"
#include "shared.h"


static char *path;

//\\// ----------------------------------------------------------------//\\//
static bool pmpath = false; // Accept the path of the pm project // -p
static bool verbose = false; // Print the process                // -v
//\\// ----------------------------------------------------------------//\\//

// Verify the project and call its Makefile
int build_project(const char *root) {
  char pmpath_file[1024];
  char makefile[1024];

  snprintf(pmpath_file, sizeof(pmpath_file), "%s/.pm", root);
  snprintf(makefile, sizeof(makefile), "%s/Makefile", root);

  FILE *f = fopen(pmpath_file, "r");
  if (!f) {
    ERROR("The path '%s' is not a pm project", root);
    return EXIT_FAILURE;
  }
  fclose(f);

  f = fopen(makefile, "r");
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
  get_flags(argc, argv);

  if (has_flag('p')) {
    pmpath = true;
    path = argv[argc - 1];
  } else {
    path = ".";
  }
  if (has_flag('v')) {
    verbose = true;
  }

  const char *flags = "pv";
  warn_invalid_flags(strlen(flags), flags);

  if (pmpath && is_flag(path)) {
    ERROR("The -p flag needs a project path");
    return EXIT_FAILURE;
  }

  return build_project(path);
}
