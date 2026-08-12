#define _XOPEN_SOURCE 700

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <log.h>

#include "config.h"
#include "shared.h"

/* Build the paths used by pm from the user's HOME directory. */
static int config_paths(char *directory, char *projects) {
  const char *home = getenv("HOME");

  if (!home || !home[0]) {
    ERROR("The HOME environment variable is not defined");
    return EXIT_FAILURE;
  }

  snprintf(directory, PM_PATH_SIZE, "%s/.config/pm", home);
  snprintf(projects, PM_PATH_SIZE, "%s/projects", directory);
  return EXIT_SUCCESS;
}

int ensure_pm_config(void) {
  char directory[PM_PATH_SIZE], projects[PM_PATH_SIZE];
  char config_directory[PM_PATH_SIZE];

  if (config_paths(directory, projects) == EXIT_FAILURE)
    return EXIT_FAILURE;

  snprintf(config_directory, sizeof(config_directory), "%s/.config",
           getenv("HOME"));

  if (mkdir(config_directory, 0755) == -1 && errno != EEXIST) {
    ERROR("Could not create '%s': %s", config_directory, strerror(errno));
    return EXIT_FAILURE;
  }
  if (mkdir(directory, 0755) == -1 && errno != EEXIST) {
    ERROR("Could not create '%s': %s", directory, strerror(errno));
    return EXIT_FAILURE;
  }

  FILE *f = fopen(projects, "a");
  if (!f) {
    ERROR("Could not create '%s': %s", projects, strerror(errno));
    return EXIT_FAILURE;
  }
  fclose(f);
  return EXIT_SUCCESS;
}

/* Copy every entry except name to a temporary file. */
static int copy_without_project(FILE *source, FILE *destination,
                                const char *name) {
  char line[PM_PATH_SIZE * 2];
  char key[256];

  while (fgets(line, sizeof(line), source)) {
    if (sscanf(line, "%255[^=]", key) == 1 && strcmp(key, name) == 0)
      continue;
    fputs(line, destination);
  }
  return EXIT_SUCCESS;
}

int register_project(const char *name, const char *path) {
  char directory[PM_PATH_SIZE], projects[PM_PATH_SIZE];
  char temporary[PM_PATH_SIZE + 8];

  if (!name || !path || strchr(name, '=')) {
    ERROR("Invalid project name or path");
    return EXIT_FAILURE;
  }
  if (ensure_pm_config() == EXIT_FAILURE ||
      config_paths(directory, projects) == EXIT_FAILURE)
    return EXIT_FAILURE;

  snprintf(temporary, sizeof(temporary), "%s.tmp", projects);
  FILE *source = fopen(projects, "r");
  FILE *destination = fopen(temporary, "w");
  if (!source || !destination) {
    ERROR("Could not update the project registry");
    if (source) fclose(source);
    if (destination) fclose(destination);
    return EXIT_FAILURE;
  }

  copy_without_project(source, destination, name);
  fprintf(destination, "%s=%s\n", name, path);
  fclose(source);
  fclose(destination);

  if (rename(temporary, projects) == -1) {
    ERROR("Could not save the project registry: %s", strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int unregister_project(const char *name) {
  char directory[PM_PATH_SIZE], projects[PM_PATH_SIZE];
  char temporary[PM_PATH_SIZE + 8];

  if (ensure_pm_config() == EXIT_FAILURE ||
      config_paths(directory, projects) == EXIT_FAILURE)
    return EXIT_FAILURE;

  snprintf(temporary, sizeof(temporary), "%s.tmp", projects);
  FILE *source = fopen(projects, "r");
  FILE *destination = fopen(temporary, "w");
  if (!source || !destination) {
    ERROR("Could not update the project registry");
    if (source) fclose(source);
    if (destination) fclose(destination);
    return EXIT_FAILURE;
  }

  copy_without_project(source, destination, name);
  fclose(source);
  fclose(destination);
  return rename(temporary, projects) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int resolve_project(const char *name, char *path, size_t size) {
  char directory[PM_PATH_SIZE], projects[PM_PATH_SIZE];
  char line[PM_PATH_SIZE * 2], key[256], value[PM_PATH_SIZE];

  if (!name || ensure_pm_config() == EXIT_FAILURE ||
      config_paths(directory, projects) == EXIT_FAILURE)
    return EXIT_FAILURE;

  FILE *f = fopen(projects, "r");
  if (!f)
    return EXIT_FAILURE;

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%255[^=]=%1023[^\n]", key, value) == 2 &&
        strcmp(key, name) == 0) {
      snprintf(path, size, "%s", value);
      fclose(f);
      return EXIT_SUCCESS;
    }
  }

  fclose(f);
  ERROR("Project '%s' is not registered", name);
  return EXIT_FAILURE;
}

int current_project(char *path, size_t size) {
  char directory[PM_PATH_SIZE], projects[PM_PATH_SIZE], current[PM_PATH_SIZE];
  char line[PM_PATH_SIZE * 2], key[256], value[PM_PATH_SIZE];

  if (!realpath(".", current)) {
    ERROR("Could not resolve the current path: %s", strerror(errno));
    return EXIT_FAILURE;
  }
  if (ensure_pm_config() == EXIT_FAILURE ||
      config_paths(directory, projects) == EXIT_FAILURE)
    return EXIT_FAILURE;

  FILE *f = fopen(projects, "r");
  if (!f)
    return EXIT_FAILURE;

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%255[^=]=%1023[^\n]", key, value) == 2 &&
        strncmp(current, value, strlen(value)) == 0 &&
        (current[strlen(value)] == '\0' || current[strlen(value)] == '/')) {
      snprintf(path, size, "%s", value);
      fclose(f);
      return EXIT_SUCCESS;
    }
  }

  fclose(f);
  ERROR("The current directory is not a registered pm project");
  return EXIT_FAILURE;
}

int read_project_target(const char *path, char *target, size_t size) {
  char config[PM_PATH_SIZE], line[1024], value[1024];

  snprintf(config, sizeof(config), "%s/.pm", path);
  FILE *f = fopen(config, "r");
  if (!f) {
    snprintf(target, size, "a");
    return EXIT_SUCCESS;
  }

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "target=%1023[^\n]", value) == 1) {
      snprintf(target, size, "%s", value);
      fclose(f);
      return EXIT_SUCCESS;
    }
  }

  fclose(f);
  snprintf(target, size, "a");
  return EXIT_SUCCESS;
}

int write_project_target(const char *path, const char *target) {
  char config[PM_PATH_SIZE];
  snprintf(config, sizeof(config), "%s/.pm", path);

  FILE *f = fopen(config, "w");
  if (!f) {
    ERROR("Could not write '%s': %s", config, strerror(errno));
    return EXIT_FAILURE;
  }
  fprintf(f, "target=%s\n", target);
  fclose(f);
  return EXIT_SUCCESS;
}

/* Print all registered projects. */
static int list_projects(void) {
  char directory[PM_PATH_SIZE], projects[PM_PATH_SIZE], line[PM_PATH_SIZE * 2];

  if (ensure_pm_config() == EXIT_FAILURE ||
      config_paths(directory, projects) == EXIT_FAILURE)
    return EXIT_FAILURE;

  FILE *f = fopen(projects, "r");
  if (!f)
    return EXIT_FAILURE;
  while (fgets(line, sizeof(line), f))
    printf("%s", line);
  fclose(f);
  return EXIT_SUCCESS;
}

int pm_config(int argc, char **argv) {
  char path[PM_PATH_SIZE], target[1024], absolute[PM_PATH_SIZE];
  char *project, *new_target;

  get_flags(argc, argv);
  const char *flags = "partv";
  warn_invalid_flags(strlen(flags), flags);

  if (has_flag('a')) {
    char *name = get_flag_value(argc, argv, 'a');
    char *given_path = get_last_arg(argc, argv);
    if (!name || !given_path || is_flag(given_path)) {
      ERROR("Usage: pm config -a <name> <path>");
      return EXIT_FAILURE;
    }
    if (!realpath(given_path, absolute)) {
      ERROR("Could not resolve '%s': %s", given_path, strerror(errno));
      return EXIT_FAILURE;
    }
    if (register_project(name, absolute) == EXIT_FAILURE)
      return EXIT_FAILURE;
    SUCCESS("Project '%s' registered", name);
    return EXIT_SUCCESS;
  }

  if (has_flag('r')) {
    project = get_flag_value(argc, argv, 'r');
    if (!project) {
      ERROR("The -r flag needs a project name");
      return EXIT_FAILURE;
    }
    return unregister_project(project);
  }

  project = get_flag_value(argc, argv, 'p');
  if (!project)
    return list_projects();
  if (resolve_project(project, path, sizeof(path)) == EXIT_FAILURE)
    return EXIT_FAILURE;

  new_target = get_flag_value(argc, argv, 't');
  if (new_target && write_project_target(path, new_target) == EXIT_FAILURE)
    return EXIT_FAILURE;

  read_project_target(path, target, sizeof(target));
  printf("project=%s\npath=%s\ntarget=%s\n", project, path, target);
  return EXIT_SUCCESS;
}
