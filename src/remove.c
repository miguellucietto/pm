#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "config.h"
#include "remove.h"
#include "shared.h"

static char path[PM_PATH_SIZE];
static bool force = false;   // Ignore missing files        // -f
static bool pmpath = false;  // Select a registered project // -p
static bool verbose = false; // Print the process           // -v
static bool noc = false;     // Do not remove source        // -c
static bool noh = false;     // Do not remove header        // -h

static int remove_one(const char *file) {
  if (remove(file) == 0)
    return EXIT_SUCCESS;
  if (force)
    return EXIT_SUCCESS;
  ERROR("Could not remove '%s'", file);
  return EXIT_FAILURE;
}

int remove_file(const char *file) {
  char fh[PM_PATH_SIZE * 2], fc[PM_PATH_SIZE * 2];

  if (!file) {
    ERROR("Provide a name to remove");
    return EXIT_FAILURE;
  }

  snprintf(fh, sizeof(fh), "%s/include/%s.h", path, file);
  snprintf(fc, sizeof(fc), "%s/src/%s.c", path, file);

  if (verbose) INFO("Removing module '%s' from project", file);

  if (!noc && remove_one(fc) == EXIT_FAILURE)
    return EXIT_FAILURE;
  if (!noh && remove_one(fh) == EXIT_FAILURE)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int pm_remove(int argc, char **argv) {
  char *project;

  get_flags(argc, argv);
  noc = has_flag('c');
  noh = has_flag('h');
  force = has_flag('f');
  pmpath = has_flag('p');
  verbose = has_flag('v');

  const char *flags = "chfpv";
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

  for (int i = 0; i < argc; i++) {
    if (i > 0 && strcmp(argv[i - 1], "-p") == 0)
      continue;
    if (!is_flag(argv[i]) && remove_file(argv[i]) == EXIT_FAILURE)
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
