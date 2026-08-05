#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "remove.h"
#include "shared.h"

void pm_remove(const char *file) {
  if (!file) {
    return;
  }

  FILE *f = fopen(".pm", "r");
  if (!f) {
    printf("[INFO] Cannot delete files outside of pm project yet");
    return;
  }

  size_t s = strlen(file) + 100;
  char fh[s], fc[s];

  snprintf(fh, sizeof(fh), "include/%s.h", file);
  snprintf(fc, sizeof(fc), "src/%s.c", file);

  CMDF("rm -rf %s", fh);
  CMDF("rm -rf %s", fc);
}
