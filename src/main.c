#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "add.h"
#include "remove.h"

typedef int (*CmdHandler)(int argc, char** argv);

typedef struct Command Command;
typedef struct Command {
  const char *name;
  const char *description;
  const char *usage;

  CmdHandler handler; 
} Command;

static const Command commands[] = {
    {"init", "make a directory with a project structure", "pm init <name>", pm_init},
    {"add", "add .h and .c files in the project", "pm add <name>", pm_add},
    {"rm", "remove .h and .c files in the project", "pm rm <name>", pm_remove},
};
#define CMDC (sizeof(commands) / sizeof(commands[0]))

#define UNIMPLEMENTED							\
  printf("Function in line %d wasn't implemented yet\n", __LINE__)


const Command *find_cmd(char* arg) {
  if (!arg)
    return NULL;
  for (int i = 0; i < (int) CMDC; i++) {
    if (strcmp(arg, commands[i].name) == 0) return &commands[i];
  }
  return NULL;
}


int main(int argc, char **argv) {
  
  if (argc < 2) {
    fprintf(stderr, "A command is necessary to run pm\n");
    return 1;
  }

  char *command = argv[1];
  const Command *cmd = find_cmd(command);
  if (!cmd) {
    fprintf(
        stderr,
        "Command '%s' not found, use the command 'help' to see other options\n",
        command);
    return EXIT_FAILURE;
  }

  return cmd->handler(argc - 2, argv + 2);
}
