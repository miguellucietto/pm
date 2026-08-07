#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "remove.h"
#include "shared.h"





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
  if (has_flag('p')) {
    
  }
}
