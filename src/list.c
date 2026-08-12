#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <log.h>

#include "config.h"
#include "list.h"
#include "shared.h"

/* Print a separator that follows the calculated table width. */
static void print_separator(size_t size) {
  for (size_t i = 0; i < size; i++)
    putchar('-');
  putchar('\n');
}

/* Return whether a registered path still points to a directory. */
static bool project_exists(const char *path) {
  struct stat info;
  return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

/* List every project registered in ~/.config/pm/projects. */
int pm_list(int argc, char **argv) {
  char registry[PM_PATH_SIZE], line[PM_PATH_SIZE * 2];
  char name[256], path[PM_PATH_SIZE], target[1024];
  size_t name_width = strlen("PROJECT");
  size_t target_width = strlen("TARGET");
  size_t path_width = strlen("PATH");
  int count = 0;
  bool show_target;

  get_flags(argc, argv);
  show_target = has_flag('t');

  const char *flags = "t";
  warn_invalid_flags(strlen(flags), flags);

  if (argc > 1 || (argc == 1 && strcmp(argv[0], "-t") != 0)) {
    ERROR("Usage: pm list [-t]");
    return EXIT_FAILURE;
  }

  if (get_projects_file_path(registry, sizeof(registry)) == EXIT_FAILURE)
    return EXIT_FAILURE;

  FILE *f = fopen(registry, "r");
  if (!f) {
    ERROR("Could not open the project registry");
    return EXIT_FAILURE;
  }

  /* First pass: count projects and calculate aligned column sizes. */
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%255[^=]=%1023[^\n]", name, path) != 2)
      continue;

    size_t length = strlen(name);
    if (length > name_width)
      name_width = length;

    length = strlen(path) + (project_exists(path) ? 0 : strlen("  [missing]"));
    if (length > path_width)
      path_width = length;

    if (show_target) {
      read_project_target(path, target, sizeof(target));
      length = strlen(target);
      if (length > target_width)
        target_width = length;
    }
    count++;
  }

  if (count == 0) {
    fclose(f);
    puts("No pm projects are registered.");
    puts("Register one with: pm config -a <name> <path>");
    return EXIT_SUCCESS;
  }

  rewind(f);
  size_t table_width = name_width + 3 + path_width;
  if (show_target)
    table_width += target_width + 3;
  if (table_width < 64)
    table_width = 64;

  printf("\nPM PROJECTS  (%d registered)\n", count);
  print_separator(table_width);
  if (show_target)
    printf("%-*s   %-*s   %s\n", (int)name_width, "PROJECT",
           (int)target_width, "TARGET", "PATH");
  else
    printf("%-*s   %s\n", (int)name_width, "PROJECT", "PATH");
  print_separator(table_width);

  /* Second pass: print the rows using the widths calculated above. */
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%255[^=]=%1023[^\n]", name, path) != 2)
      continue;

    const char *status = project_exists(path) ? "" : "  [missing]";
    if (show_target) {
      read_project_target(path, target, sizeof(target));
      printf("%-*s   %-*s   %s%s\n", (int)name_width, name,
             (int)target_width, target, path, status);
    } else {
      printf("%-*s   %s%s\n", (int)name_width, name, path, status);
    }
  }

  print_separator(table_width);
  fclose(f);
  return EXIT_SUCCESS;
}
