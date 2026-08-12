#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <log.h>

#include "build.h"
#include "shared.h"

/*
 * Build flags:
 *
 *   -p <path>  build a project outside the current directory
 *   -v         show the command executed by make
 */
static bool verbose = false;
static bool pmpath = false;

static bool is_pm_project(const char *root) {
  char marker[1024];

  if (snprintf(marker, sizeof(marker), "%s/.pm", root) >= (int)sizeof(marker))
    return false;

  return access(marker, F_OK) == 0;
}

int build_project(const char *root, int show_commands) {
  char makefile[1024];

  if (!root)
    root = ".";

  if (!is_pm_project(root)) {
    ERROR("The path '%s' is not a pm project", root);
    return EXIT_FAILURE;
  }

  if (snprintf(makefile, sizeof(makefile), "%s/Makefile", root) >=
      (int)sizeof(makefile)) {
    ERROR("Project path is too long: '%s'", root);
    return EXIT_FAILURE;
  }

  if (access(makefile, R_OK) != 0) {
    ERROR("Could not find a readable Makefile in '%s'", root);
    return EXIT_FAILURE;
  }

  INFO("Building project in '%s'", root);

  if (show_commands)
    return CMD("make", "-C", (char *)root, "all");

  return CMD("make", "--no-print-directory", "-s", "-C", (char *)root,
             "all");
}

int pm_build(int argc, char **argv) {
  const char *root = ".";

  /* Reset static state because handlers may also be called from tests. */
  verbose = false;
  pmpath = false;

  get_flags(argc, argv);
  verbose = has_flag('v');
  pmpath = has_flag('p');

  const char *flags = "pv";
  warn_invalid_flags((int)strlen(flags), flags);

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0) {
      if (++i >= argc) {
        ERROR("The -p flag requires a project path");
        return EXIT_FAILURE;
      }
      root = argv[i];
      continue;
    }

    if (is_flag(argv[i])) {
      /* Keep the grouped-flag style used by the other pm commands. */
      for (int j = 1; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'v')
          verbose = true;
      }
      continue;
    }

    ERROR("Unexpected build argument: '%s'", argv[i]);
    return EXIT_FAILURE;
  }

  if (pmpath && strcmp(root, ".") == 0) {
    ERROR("The -p flag requires a project path");
    return EXIT_FAILURE;
  }

  return build_project(root, verbose);
}
