#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern bool Flags[256];

/*
 * Execute a formatted shell command.
 *
 * Prefer CMD from log.h for new code: it avoids the shell, reports the exit
 * status and produces a structured command log. CMDF remains for legacy code.
 */
#define CMDF(format, ...)                                                      \
  do {                                                                         \
    char command_buffer[1024];                                     \
    snprintf(command_buffer, 1024, format, ##__VA_ARGS__);         \
    system(command_buffer);                                                    \
  } while (0)


/* Contents installed by `pm init` when the matching files are enabled. */
extern const char DEFAULT_MAIN_C[];
extern const char DEFAULT_CLANGD[];
extern const char DEFAULT_MAKEFILE[];



/* Return whether a flag was collected by the latest get_flags() call. */
bool has_flag(char flag);

/* Return whether command has the syntactic form of an option. */
bool is_flag(char* command);

/* Reset the global flag table and collect every character after '-'. */
void get_flags(int argc, char **argv);

/* Log flags that are absent from the supplied list of allowed characters. */
void warn_invalid_flags(int flagc, const char* flags);

/* Safely retrieve argv[i]; returns NULL when i is outside the argument list. */
char *get_arg(int argc, char **argv, int i);

/* Allocate a copy of text; the caller must release it with free(). */
char* strdup(const char*);

#endif
