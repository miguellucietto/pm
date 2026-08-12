#ifndef RUN_H
#define RUN_H

#include <stdbool.h>
#include <stddef.h>

int pm_run(int argc, char** argv);
int run_project(const char *name, int argc, char **argv);
bool is_executable(const char *file);
int find_executable(char *executable, size_t size);

#endif // RUN_H
