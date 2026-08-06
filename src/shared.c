#include <stdbool.h>
#include <string.h>
#include "shared.h"


char *get_arg(int argc, char** argv, int i) {
  if (argc < i || i < 0)
    return NULL;

  return argv[i];
}

bool has_flag(int argc, char** argv, char flag) {
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; j < (int) strlen(argv[i]); j++) {
	if (argv[i][j] == flag) return true;
      }
    }
  }
  return false;
}
