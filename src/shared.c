#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include "shared.h"






char *get_arg(int argc, char** argv, int i) {
  if (argc < i || i < 0)
    return NULL;

  return argv[i];
}


// Flags
bool Flags[256] = {false};

bool is_flag(char* command) {
  if (command[0] == '-')
    return true;
}

bool has_flag(char flag) {
  return Flags[(unsigned char) flag];
}


void get_flags(int argc, char **argv) { 
  memset(Flags, 0, sizeof(Flags));

 for (int i = 0; i < argc; ++i) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0'; j++) {
	Flags[(unsigned char)argv[i][j]] = true;
      }
    }
  }
}



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
    fprintf(stderr, "Unrecognized flags '%s'\n", nflags);
  }
}
