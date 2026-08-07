#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "init.h"
#include "shared.h"


bool no_bear = false;           // Cancel the compile_commands.json // -b
bool no_default_files = false; // Cancel the default files         // -d
bool no_make = false;         // Cancel the Makefile              // -M
bool no_cland = false;       // Cancel the .clangd               // -c
bool no_main = false;       // Cancel the main.c                // -m


int make_path(char *path, size_t size, const char* r, const char* t) {
  if (!path || !t) {
    fprintf(stderr, "Could not resolve path (%s)", __func__);
    return EXIT_FAILURE;
  }
  const char *root = r ? r : ".";
  snprintf(path, size, "%s/%s", root, t);
  return EXIT_SUCCESS;
}

int write_file(const char* path, const char* content) {
  if (!content) {
    fprintf(stderr, "Provide a content to write in the file: %s (%s)", path, __func__);
    return EXIT_FAILURE;
  }
  if (!path) {
    fprintf(stderr, "Provide a path to write the content (%s)", __func__);
    return EXIT_FAILURE;
  }

  FILE *f = fopen(path, "w");
  if (!f) {
    fprintf(stderr, "Could not open file: %s (%s)", path, __func__);
    return EXIT_FAILURE;
  }
  fputs(content, f);
  fclose(f);

  return EXIT_SUCCESS;
}


int write_default_files(const char* r) {
  const char* root = r ? r : ".";

  char path[1024];
  
#define W(f, content)                                                          \
  do {                                                                         \
    make_path(path, sizeof(path), root, f);					\
    if (write_file(path, content) == EXIT_FAILURE)                                             \
      return EXIT_FAILURE;                                                               \
  } while (0)
  

  W("Makefile", DEFAULT_MAKEFILE);
  W(".clangd", DEFAULT_CLANGD);
  W("src/main.c", DEFAULT_MAIN_C);

  return EXIT_SUCCESS;
}


int init(const char* root){
char path[1024];
  
  if (root && mkdir(root, 0777) && errno != EEXIST) {
    fprintf(stderr, "Could not resolve root (%s)", __func__);
      return EXIT_FAILURE;
  }

#define Mkdir(dir)                                                             \
  do {                                                                         \
    make_path(path, sizeof(path), root, dir);                                  \
    if (mkdir(path, 0777) && errno != EEXIST)                                  \
      return EXIT_FAILURE;                                                               \
  } while (0)

  snprintf(path, sizeof(path), "%s/.pm", root);
  FILE *f = fopen(path, "w");
  fputs("This file is necessary for some pm commands, pls, keep it here", f);
  fclose(f);

  Mkdir("src");
  Mkdir("include");
  Mkdir("build");
  
  return write_default_files(root);
}


int pm_init(int argc, char** argv) {
  get_flags(argc, argv);
  if (has_flag('c')) {
    no_cland = true;
  }
  if (has_flag('m')) {
    no_main = true;
  }
  if (has_flag('M')) {
    no_make = true;
  }
  if (has_flag('d')) {
    no_default_files = true;
  }
  if (has_flag('b')) {
    no_bear = true;
  }
  

  for (int i = 0; i < argc; i++) {
    if (!is_flag(argv[i]) && init(argv[i]) == EXIT_FAILURE)
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
