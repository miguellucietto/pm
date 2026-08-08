#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern bool Flags[256];

#define CMDF(format, ...)                                                      \
  do {                                                                         \
    char command_buffer[1024];                                     \
    snprintf(command_buffer, 1024, format, ##__VA_ARGS__);         \
    system(command_buffer);                                                    \
  } while (0)


static const char *DEFAULT_MAIN_C =
"#include <stdio.h>\n"
"\n"
"int main(void)\n"
"{\n"
"    return 0;\n"
"}\n";

static const char *DEFAULT_CLANGD =
"CompileFlags:\n"
"  Add:\n"
"    - -Wall\n"
"    - -Wextra\n"
"    - -Wpedantic\n"
"    - -std=c17\n"
"    - -ggdb\n"
"    - -Iinclude\n";

static const char *DEFAULT_MAKEFILE =
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



// Functions
bool has_flag(char flag);
bool is_flag(char* command);
void get_flags(int argc, char **argv);
void warn_invalid_flags(int flagc, const char* flags);
char *get_arg(int argc, char **argv, int i);
char* strdup(const char*);

#endif
