#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define PM_PATH_SIZE 1024

/* CLI handler for `pm config`. */
int pm_config(int argc, char **argv);

/* Create ~/.config/pm and its projects file when they do not exist. */
int ensure_pm_config(void);

/* Register or update a project name with its absolute path. */
int register_project(const char *name, const char *path);

/* Remove a name from the global project registry. */
int unregister_project(const char *name);

/* Resolve a registered project name to its absolute path. */
int resolve_project(const char *name, char *path, size_t size);

/* Find which registered project contains the current directory. */
int current_project(char *path, size_t size);

/* Read or write the target stored in <project>/.pm. */
int read_project_target(const char *path, char *target, size_t size);
int write_project_target(const char *path, const char *target);

#endif // CONFIG_H
