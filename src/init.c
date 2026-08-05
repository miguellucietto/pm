#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "init.h"
#include "shared.h"

void make_path(char *path, size_t size, const char* r, const char* t) {
  if (!path || !t) {
    fprintf(stderr, "Could not resolve path (%s)", __func__);
    return;
  }
  const char *root = r ? r : ".";
  snprintf(path, size, "%s/%s", root, t);
}

int write_file(const char* path, const char* content) {
  if (!content) {
    fprintf(stderr, "Provide a content to write in the file: %s (%s)", path, __func__);
    return 1;
  }
  if (!path) {
    fprintf(stderr, "Provide a path to write the content (%s)", __func__);
    return -1;
  }

  FILE *f = fopen(path, "w");
  if (!f) {
    fprintf(stderr, "Could not open file: %s (%s)", path, __func__);
    return 1;
  }
  fputs(content, f);
  fclose(f);

  return 0;
}


int write_default_files(const char* r) {
  const char* root = r ? r : ".";

  char path[1024];
  
#define W(f, content)                                                          \
  do {                                                                         \
    make_path(path, sizeof(path), root, f);					\
    if (write_file(path, content))                                             \
      return 1;                                                               \
  } while (0)
  

  W("Makefile", DEFAULT_MAKEFILE);
  W(".clangd", DEFAULT_CLANGD);
  W("src/main.c", DEFAULT_MAIN_C);

  return 0;
}

int pm_init(const char *root) {
  char path[1024];
  
  if (root && mkdir(root, 0777) && errno != EEXIST) {
    fprintf(stderr, "Could not resolve root (%s)", __func__);
      return -1;
  }

#define Mkdir(dir)                                                             \
  do {                                                                         \
    make_path(path, sizeof(path), root, dir);                                  \
    if (mkdir(path, 0777) && errno != EEXIST)                                  \
      return -1;                                                               \
  } while (0)


  FILE *f = fopen(".pm", "w");
  fprintf(f, "pm\n\n\nThis file is necessary to recognize this directory as a pm directory\nPlease do not remove it from your project\notherwise, some commands may not work");
  fclose(f);

  Mkdir("src");
  Mkdir("include");
  Mkdir("build");
  
  return write_default_files(root);
}
