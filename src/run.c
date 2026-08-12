#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <log.h>

#include "build.h"
#include "run.h"
#include "shared.h"

/*
 * Run flags:
 *
 *   -p <path>  use a project outside the current directory
 *   -b         build the project before running it
 *   -v         print extra information (and verbose build output)
 *   -a         everything after this flag is passed to the executable
 *
 * The executable name is optional. Without it, pm searches the project root
 * for a single executable binary. If more than one is found, the user must
 * provide its name explicitly.
 */
static bool verbose = false;
static bool pmpath = false;
static bool build = false;
static bool args = false;

static bool is_pm_project(const char *root) {
  char marker[1024];

  if (snprintf(marker, sizeof(marker), "%s/.pm", root) >= (int)sizeof(marker))
    return false;

  return access(marker, F_OK) == 0;
}

/* Check the ELF magic number so object files and text scripts are ignored. */
static bool is_elf_executable(const char *file) {
  static const unsigned char elf_magic[] = {0x7f, 'E', 'L', 'F'};
  unsigned char magic[sizeof(elf_magic)];
  struct stat info;

  if (stat(file, &info) != 0 || !S_ISREG(info.st_mode) ||
      access(file, X_OK) != 0)
    return false;

  FILE *stream = fopen(file, "rb");
  if (!stream)
    return false;

  size_t read = fread(magic, 1, sizeof(magic), stream);
  fclose(stream);

  return read == sizeof(magic) &&
         memcmp(magic, elf_magic, sizeof(elf_magic)) == 0;
}

static int make_executable_path(char *result, size_t size, const char *root,
                                const char *name) {
  if (strchr(name, '/')) {
    if (name[0] == '/')
      return snprintf(result, size, "%s", name) < (int)size ? EXIT_SUCCESS
                                                            : EXIT_FAILURE;
    return snprintf(result, size, "%s/%s", root, name) < (int)size
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
  }

  return snprintf(result, size, "%s/%s", root, name) < (int)size
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}

/* Find exactly one executable ELF file in the root of the project. */
static int find_executable(const char *root, char *result, size_t size) {
  DIR *directory = opendir(root);
  struct dirent *entry;
  int matches = 0;

  if (!directory) {
    ERROR("Could not open project path '%s': %s", root, strerror(errno));
    return EXIT_FAILURE;
  }

  while ((entry = readdir(directory))) {
    char candidate[1024];

    if (entry->d_name[0] == '.')
      continue;
    if (make_executable_path(candidate, sizeof(candidate), root,
                             entry->d_name) != EXIT_SUCCESS)
      continue;
    if (!is_elf_executable(candidate))
      continue;

    matches++;
    if (matches == 1)
      snprintf(result, size, "%s", candidate);
  }

  closedir(directory);

  if (matches == 0) {
    ERROR("No executable binary was found in '%s'; build the project first",
          root);
    return EXIT_FAILURE;
  }
  if (matches > 1) {
    ERROR("More than one executable was found in '%s'; provide its name", root);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static int execute_project(const char *executable, int argument_count,
                           char **arguments) {
  char **command = calloc((size_t)argument_count + 2, sizeof(*command));

  if (!command) {
    ERROR("Could not allocate the executable argument list");
    return EXIT_FAILURE;
  }

  command[0] = (char *)executable;
  for (int i = 0; i < argument_count; i++)
    command[i + 1] = arguments[i];
  command[argument_count + 1] = NULL;

  int status = cmd_run(command, __FILE__, __LINE__, __func__);
  free(command);
  return status;
}

int pm_run(int argc, char **argv) {
  const char *root = ".";
  const char *name = NULL;
  char **program_args = NULL;
  int program_argc = 0;
  char executable[1024];

  verbose = false;
  pmpath = false;
  build = false;
  args = false;

  /*
   * Parse only pm arguments before -a. Program arguments may themselves begin
   * with '-', so they must not be interpreted as pm flags.
   */
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-a") == 0) {
      args = true;
      program_args = argv + i + 1;
      program_argc = argc - i - 1;
      break;
    }
    if (strcmp(argv[i], "-p") == 0) {
      pmpath = true;
      if (++i >= argc) {
        ERROR("The -p flag requires a project path");
        return EXIT_FAILURE;
      }
      root = argv[i];
      continue;
    }
    if (is_flag(argv[i])) {
      bool valid = true;

      /* -v and -b may be grouped as -vb, matching the rest of pm. */
      for (int j = 1; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'v')
          verbose = true;
        else if (argv[i][j] == 'b')
          build = true;
        else
          valid = false;
      }

      if (!valid)
        WARN("Unrecognized run flag '%s'", argv[i]);
      continue;
    }
    if (name) {
      ERROR("Unexpected run argument '%s'; use -a before program arguments",
            argv[i]);
      return EXIT_FAILURE;
    }
    name = argv[i];
  }

  if (!is_pm_project(root)) {
    ERROR("The path '%s' is not a pm project", root);
    return EXIT_FAILURE;
  }

  if (build && build_project(root, verbose) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (name) {
    if (make_executable_path(executable, sizeof(executable), root, name) !=
        EXIT_SUCCESS) {
      ERROR("Executable path is too long");
      return EXIT_FAILURE;
    }
    if (!is_elf_executable(executable)) {
      ERROR("'%s' is not an executable binary", executable);
      return EXIT_FAILURE;
    }
  } else if (find_executable(root, executable, sizeof(executable)) !=
             EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  if (verbose) {
    INFO("Project path: '%s'", root);
    INFO("Executable: '%s'", executable);
    INFO("Program arguments: %d", args ? program_argc : 0);
  }

  return execute_project(executable, program_argc, program_args);
}
