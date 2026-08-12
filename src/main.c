#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "init.h"
#include "add.h"
#include "remove.h"
#include "build.h"
#include "run.h"

typedef int (*CmdHandler)(int argc, char** argv);

/* Metadata used to match a command name with its implementation. */
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
    {"build", "build a pm project", "pm build [-v] [-p <path>]", pm_build},
    {"run", "run a pm project", "pm run [-v] [-b] [-p <path>] [name] [-a args...]", pm_run},
};
#define CMDC (sizeof(commands) / sizeof(commands[0]))

#define UNIMPLEMENTED							\
  printf("Function in line %d wasn't implemented yet\n", __LINE__)


/* Return the command metadata matching arg, or NULL when it is unknown. */
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
    ERROR("A command is necessary to run pm");
    return EXIT_FAILURE;
  }

  char *command = argv[1];
  const Command *cmd = find_cmd(command);
  if (!cmd) {
    ERROR("Command '%s' not found, use the command 'help' to see other options",
          command);
    return EXIT_FAILURE;
  }

  return cmd->handler(argc - 2, argv + 2);
}
