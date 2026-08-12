#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <log.h>

#include "init.h"
#include "shared.h"

static char* path;

/* Flags accepted by `pm init`. */
static bool pmpath = false; // Accept the path of the directory to create the project  -p | path
static bool no_default_files = false; // Cancel the default files                      -d | default files
static bool no_make = false; // Cancel the Makefile                                    -M | Makefile
static bool no_cland = false; // Cancel the .clangd                                    -c | .clangd
static bool no_main = false; // Cancel the main.c                                      -m | main
static bool verbose = false; // Print the process                                      -v | verbose


/* Join a project root and a relative path into the caller's buffer. */
int make_path(char *p, size_t size, const char* r, const char* t) {
  if (!p || !t) {
    ERROR("Could not resolve path");
    return EXIT_FAILURE;
  }
  
  const char *root = r ? r : ".";

  if (verbose)
    printf("Making path '%s/%s'\n", root, t);

  // Making path
  snprintf(p, size, "%s/%s", root, t);

  if (verbose) puts("done");
  
  return EXIT_SUCCESS;
}

/* Create or replace a text file with the provided default content. */
int write_file(const char* p, const char* content) {
  if (!content) {
    ERROR("No content was provided for file '%s'", p ? p : "(null)");
    return EXIT_FAILURE;
  }
  if (!p) {
    ERROR("No path was provided for the file");
    return EXIT_FAILURE;
  }

  // Opening the file
  if (verbose) printf("Opening file '%s'\n", p);
  
  FILE *f = fopen(p, "w");
  if (!f) {
    ERROR("Could not open file '%s': %s", p, strerror(errno));
    return EXIT_FAILURE;
  }

  // Writing the content in the file
  if (verbose) printf("Writing default content in file '%s'\n", p);

  fputs(content, f);
  
  fclose(f);

  return EXIT_SUCCESS;
}


/* Write every enabled file from the project template. */
int write_default_files(const char* r) {
  const char* root = r ? r : ".";

  char p[1024];
  
#define W(f, content)                                                          \
  do {                                                                         \
    make_path(p, sizeof(p), root, f);					\
    if (write_file(p, content) == EXIT_FAILURE)                                             \
      return EXIT_FAILURE;                                                               \
  } while (0)

  if (!no_default_files) {
    if (verbose) printf("Making default files: %s | %s | %s\n", no_make ? "" : "Makefile", no_cland ? "" : ".clangd", no_main ? "" : "main.c");
    if (!no_make)  W("Makefile", DEFAULT_MAKEFILE);
    if (!no_cland) W(".clangd", DEFAULT_CLANGD);
    if (!no_main)  W("src/main.c", DEFAULT_MAIN_C);
  }

  puts("Process finished!");
  return EXIT_SUCCESS;
}


/* Create the directories, marker and templates for one pm project. */
int init(char* r){
  char p[1024], root[1024];
  if (r) {
    if (snprintf(root, sizeof(root), "%s/%s", path, r) >= (int)sizeof(root)) {
      ERROR("Project path is too long");
      return EXIT_FAILURE;
    }
  } else return EXIT_FAILURE;
  
  // Creating directory
 if (verbose) puts("Creating directory...");
  if (mkdir(root, 0777) && errno != EEXIST) {
    ERROR("Could not create project root '%s': %s", root, strerror(errno));
      return EXIT_FAILURE;
  }

#define Mkdir(dir)                                                             \
  do {                                                                         \
    make_path(p, sizeof(p), root, dir);                                  \
    if (mkdir(p, 0777) && errno != EEXIST)                                  \
      return EXIT_FAILURE;                                                               \
  } while (0)

  // Making .pm path
  if (verbose) puts("Creating .pm...");
  
  if (snprintf(p, sizeof(p), "%s/.pm", root) >= (int)sizeof(p)) {
    ERROR("Project marker path is too long");
    return EXIT_FAILURE;
  }
  FILE *f = fopen(p, "w");
  if (!f) {
    ERROR("Could not create project marker '%s': %s", p, strerror(errno));
    return EXIT_FAILURE;
  }
  fprintf(f, "%s\nThis file is necessary for some pm commands, pls, keep it here", root);
  fclose(f);


  // Making path and dirs
  if (verbose) puts("Making default dirs...");
  Mkdir("src");
  Mkdir("include");
  Mkdir("build");
  
  return write_default_files(root);
}


/* Parse `pm init` arguments and initialize every supplied project name. */
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
  if (has_flag('p')) {
    pmpath = true;
    path = argv[argc - 1];
  } else {
    path = ".";
  }
  if (has_flag('v')) {
    verbose = true;
  }


  for (int i = 0; i < argc; i++) {
    if (pmpath && i == argc - 1)
      continue;
        
    if (!is_flag(argv[i]) && init(argv[i]) == EXIT_FAILURE)
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
