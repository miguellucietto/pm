#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <log.h>

#include "shared.h"

/* Default files are defined once here and exposed read-only through shared.h. */
const char DEFAULT_MAIN_C[] =
    "#include <stdio.h>\n"
    "\n"
    "int main(void)\n"
    "{\n"
    "    return 0;\n"
    "}\n";

const char DEFAULT_CLANGD[] =
    "CompileFlags:\n"
    "  Add:\n"
    "    - -Wall\n"
    "    - -Wextra\n"
    "    - -Wpedantic\n"
    "    - -std=c17\n"
    "    - -ggdb\n"
    "    - -Iinclude\n";

const char DEFAULT_MAKEFILE[] =
    "CC = gcc\n"
    "\n"
    "CFLAGS = -Wall -Wextra -Wpedantic -std=c17 -ggdb -Iinclude -MMD -MP\n"
    "TARGET ?= a\n"
    "\n"
    "CFILES = $(wildcard src/*.c)\n"
    "OBJECTS = $(patsubst src/%.c,build/%.o,$(CFILES))\n"
    "DEPS = $(OBJECTS:.o=.d)\n"
    "\n"
    "all: $(TARGET)\n"
    "\n"
    "$(TARGET): $(OBJECTS)\n"
    "\t$(CC) $(OBJECTS) -o $@\n"
    "\n"
    "build/%.o: src/%.c\n"
    "\t@mkdir -p build\n"
    "\t$(CC) $(CFLAGS) -c $< -o $@\n"
    "\n"
    "-include $(DEPS)\n"
    "\n"
    "clean:\n"
    "\trm -rf build $(TARGET)\n"
    "\n"
    ".PHONY: all clean\n";


/* Allocate an independent copy of text. The caller owns the returned memory. */
char *strdup(const char* text) {
  if (!text)
    return NULL;

  size_t len = strlen(text) + 1;
  char *copy = malloc(len);
  if (!copy)
    return NULL;

  memcpy(copy, text, len);
  return copy;
}



/* Return an argument by index, or NULL when the index is outside argv. */
char *get_arg(int argc, char** argv, int i) {
  if (!argv || i < 0 || i >= argc)
    return NULL;

  return argv[i];
}


/* One entry for every possible unsigned-char flag name. */
bool Flags[256] = {false};

/* A pm option always starts with '-'. */
bool is_flag(char* command) {
  return command && command[0] == '-';
}

/* Query a flag previously collected by get_flags(). */
bool has_flag(char flag) {
  return Flags[(unsigned char) flag];
}


/* Collect grouped flags, so -vf is equivalent to -v -f. */
void get_flags(int argc, char **argv) {
  if (!argv) {
    PANIC("No argument or flag detected");
  }
  memset(Flags, 0, sizeof(Flags));

 for (int i = 0; i < argc; ++i) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0'; j++) {
	Flags[(unsigned char)argv[i][j]] = true;
      }
    }
  }
}



/* Log flags that are not present in the command's allowlist. */
void warn_invalid_flags(int flagc, const char* rflags) {
  char nflags[256];
  int count = 0;

  for (int i = 0; i < 256; i++) {
    if (!Flags[i])
      continue;

    bool allowed = false;
    for (int j = 0; j < flagc; j++) {
      if (i == rflags[j]) {
        allowed = true;
	break;
      }
    }
    if (!allowed) {
      nflags[count++] = i;
    }
  }
  nflags[count] = '\0';
  if (count > 0) {
    WARN("Unrecognized flags '%s'", nflags);
  }
}
