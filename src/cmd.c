#include "global.h"
#include "proj.h"
#include "cmd.h"


void exec_cmd(CMD cmd)
{
    if (!cmd) return;

    if (strcmp("create project", cmd) == 0) create_project(argv[0]);
}

CMD get_cmd(int argc, char** argv)
{
    if (strcmp(argv[2], "project") == 0) 
    {

        if (strcmp(argv[1], "new") == 0) 
            return (CMD){
        .cmd = CMD_CREATE_PROJECT,
        .arg = false,
        .count = 0,
        .args = NULL,};

        if (strcmp(argv[1], "info") == 0)
                return (CMD) {
            .cmd = CMD_INFO_PROJECT,
            .arg = false,
            .count = 0,
            .args = NULL,};

        if (strcmp(argv[1], "locate") == 0)
                return (CMD) {
            .cmd = CMD_LOCATE_PROJECT,
            .arg = false,
            .count = 0,
            .args = NULL,};
    }

    // TODO: fazer um bom parser
    // TODO: implementar as funções info e locate project
    // TODO: implementar outros comandos e funcionalidades
}
