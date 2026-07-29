#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "proj.h"
#include "cmd.h"


void exec_cmd(CMD cmd)
{
    if (strcmp(CMD_CREATE_PROJECT, cmd.cmd) == 0) {
        if (cmd.count < 1) {
            ERROR("This command needs at least one argument");
            exit(1);
        }
        for (int i = 0; i < cmd.count; i++)
            create_project(cmd.args[i]);
        return;
    }
    if (strcmp(CMD_INFO_PROJECT, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_LIST_PROJECT, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_SETUP_PROJECT, cmd.cmd) == 0) {
        setup_project();
        return;
    }
    if (strcmp(CMD_LOCATE_PROJECT, cmd.cmd) == 0) {
        if (cmd.count != 1) {
            ERROR("This command needs at least one argument");
            exit(1);
        }
        locate_project(cmd.args[0]);
        return;
    }
    if (strcmp(CMD_DELETE_PROJECT, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_START_GIT, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_SAVE_GIT, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_SETUP_ALL, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_SETUP_GIT, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_SETUP_GITHUB, cmd.cmd) == 0) {
        TODO;
        return;
    }
    if (strcmp(CMD_SETUP_PROJECT, cmd.cmd) == 0) {
        setup_project();
    }
}

CMD get_cmd(int argc, char** argv)
{
    CMD cmd = {0};

    if (strcmp(argv[2], "project") == 0)
    {
        if (strcmp(argv[1], "new") == 0)
        {
            if (!argv[3]) {
                ERROR("this command needs at least one argument");
                return cmd;
            }
            cmd.cmd = CMD_CREATE_PROJECT;
            cmd.args = argv;
            cmd.arg = true;
            cmd.count = argc;
            return cmd;
        }

        if (strcmp(argv[1], "info") == 0)
        {
            TODO;
        }

        if (strcmp(argv[1], "locate") == 0)
        {
            cmd.cmd = CMD_LOCATE_PROJECT;
            cmd.arg = false;
            cmd.count = 0;
            cmd.args = NULL;
            return cmd;
        }
    }

    TODOF("melhorar parser");
    TODOF("implementar comandos e funções");

    return cmd;
}
