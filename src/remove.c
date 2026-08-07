#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "remove.h"
#include "shared.h"


char* path;
//\\// ----------------------------------------------------------------//\\//
bool force = false; // Force the deletion of the files               // -f
bool pmpath = false; // Accept the path of the pm project           // -p
bool verbose = false; // Print the process                         // -v
bool noc = false; // Don't remove the source file                 // -c
bool noh = false; // Don't remove the header file                // -h
//\\// ----------------------------------------------------------------//\\//


int remove_file(const char* file) {
if (!file) {
    return EXIT_FAILURE;
  }

  FILE *f;
  // Verifying pm project
  if (verbose) puts("Verifying pm project...");
  if (pmpath) {
    char opath[1024];
    snprintf(opath, sizeof(opath), "%s/.pm", path);

    f = fopen(opath, "r");
    if (!f) {
      fprintf(stderr, "The path '%s' is not a pm project\n", path);
      return EXIT_FAILURE;
    }
  } else {
    f = fopen(".pm", "r");
    if (!f) {
      fprintf(stderr, "The command needs a pm project\n");
      return EXIT_FAILURE;
    }
  }
  fclose(f);

  
  size_t s = strlen(file) + 1000;
  char fh[s], fc[s];

  // Making paths
  if (verbose) puts("Making paths...");
  if (pmpath) {
    snprintf(fh, s, "%s/include/%s.c", path, file);
    if (verbose) printf("Created path '%s'", fh);
    
    snprintf(fc, s, "%s/src/%s.h", path, file);
    if (verbose) printf("Created path '%s'", fc);
    
  } else {
    snprintf(fh, s, "include/%s.h", file);
    if (verbose) printf("Created path '%s'", fh);
    
    snprintf(fc, s, "src/%s.c", file);
    if (verbose) printf("Created path '%s'", fc);
  }


  // Removing files
  if (verbose) puts("Removing files...");
  if (!noc) {
    CMDF("rm %s %s", force ? "-rf" : "", fc);
    if (verbose) printf("file '%s removed'", fc);
  }
  if (!noh) {
    CMDF("rm %s %s", force ? "-rf" : "", fh);
    if (verbose) printf("file '%s removed'", fh);
  }

  if (verbose) puts("Finished process!");

  return EXIT_SUCCESS;
}


int pm_remove(int argc, char** argv) {
  get_flags(argc, argv);
  if (has_flag('c')) {
    noc = true;
  }
  if (has_flag('h')) {
    noh = true;
  }
  if (has_flag('f')) {
    force = true;
  }
  if (has_flag('p')) {
    pmpath = true;
    path = argv[argc - 1];
  }
  if (has_flag('v')) {
    verbose = true;
  }

  const char *flags = "chfpv";
  warn_invalid_flags(strlen(flags), flags);

  for (int i = 0; i < argc; i++) {
    if (pmpath) {
      if (i == argc - 1) {
	continue;
      }
    }
    if (remove_file((const char *)argv[i]) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
