#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

#define UNIMPLEMENTED\
  printf("Function %s wasn't implemented yet\n", __func__)

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "A command is necessary to run pm\n");
    return 1;
  }

  char *command = argv[1];

#define IS(command, this) (strcmp(command, this) == 0)

  if (IS(command, "init")) {
    for (int i = 2; i < argc; i++)
      pm_init(argv[i]);
    return 0;
  } else if (IS(command, "add")) {
    UNIMPLEMENTED;
    return 0;
  } else {
    printf("Command '%s' not found", argv[1]);
    return 0;
  }
}
