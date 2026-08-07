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

  FILE *f = fopen(".pm", "r");
  if (!f) {
    printf("[INFO] Cannot delete files outside of pm project yet");
    return EXIT_FAILURE;
  }

  size_t s = strlen(file) + 100;
  char fh[s], fc[s];

  snprintf(fh, sizeof(fh), "include/%s.h", file);
  snprintf(fc, sizeof(fc), "src/%s.c", file);

  CMDF("rm -rf %s", fh);
  CMDF("rm -rf %s", fc);

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
    remove_file((const char*) argv[i]);
  }
}
