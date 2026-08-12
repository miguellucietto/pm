#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "add.h"
#include "config.h"
#include "shared.h"

static char path[PM_PATH_SIZE];
static bool pmpath = false; // Select a registered project // -p
static bool verbose = false; // Print the process           // -v
static bool force = false;   // Add files in current path   // -f
static bool noc = false;     // Do not create .c            // -h
static bool noh = false;     // Do not create .h            // -c

int add_file(const char *name) {
  char fileh[PM_PATH_SIZE * 2], filec[PM_PATH_SIZE * 2];
  FILE *f;

  if (!name) {
    ERROR("Provide a name to add");
    return EXIT_FAILURE;
  }

  snprintf(fileh, sizeof(fileh), "%s/include/%s.h", path, name);
  snprintf(filec, sizeof(filec), "%s/src/%s.c", path, name);

  if (verbose) INFO("Creating module '%s' in project", name);

  if (!noc) {
    f = fopen(filec, "w");
    if (!f) {
      ERROR("Could not create source file for '%s'", name);
      return EXIT_FAILURE;
    }
    fprintf(f, "#include \"%s.h\"", name);
    fclose(f);
  }

  if (!noh) {
    f = fopen(fileh, "w");
    if (!f) {
      ERROR("Could not create header file for '%s'", name);
      return EXIT_FAILURE;
    }
    fprintf(f, "#ifndef %s_H_\n#define %s_H_\n\n#endif /* %s_H_ */\n",
            name, name, name);
    fclose(f);
  }

  return EXIT_SUCCESS;
}

int pm_add(int argc, char **argv) {
  char *project;

  get_flags(argc, argv);
  pmpath = has_flag('p');
  verbose = has_flag('v');
  force = has_flag('f');
  noh = has_flag('h');
  noc = has_flag('c');

  const char *flags = "phcvf";
  warn_invalid_flags(strlen(flags), flags);

  project = get_flag_value(argc, argv, 'p');
  if (pmpath && !project) {
    ERROR("The -p flag needs a project name");
    return EXIT_FAILURE;
  }

  if (project) {
    if (resolve_project(project, path, sizeof(path)) == EXIT_FAILURE)
      return EXIT_FAILURE;
  } else if (force) {
    snprintf(path, sizeof(path), ".");
  } else if (current_project(path, sizeof(path)) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (noh && noc) {
    WARN("Flags -c and -h disable both files");
    return EXIT_SUCCESS;
  }

  for (int i = 0; i < argc; i++) {
    if (i > 0 && strcmp(argv[i - 1], "-p") == 0)
      continue;
    if (!is_flag(argv[i]) && add_file(argv[i]) == EXIT_FAILURE)
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
