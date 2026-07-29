#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdbool.h>

/*
 * Definições de comandos e estruturas compartilhadas pelo parser do projeto.
 * A ideia é manter aqui apenas o contrato de comandos, sem misturar com
 * macros de utilidade ou constantes específicas de configuração.
 */
typedef struct
{
    const char* cmd;
    bool arg;

    int count;
    char** args;
} CMD;

// Parsed commands

// Project
extern const char CMD_LIST_PROJECT[14];
extern const char CMD_INFO_PROJECT[14];
extern const char CMD_CREATE_PROJECT[16];
extern const char CMD_LOCATE_PROJECT[16];
extern const char CMD_DELETE_PROJECT[16];

// Git
extern const char CMD_SAVE_GIT[9];
extern const char CMD_START_GIT[10];

// Setup
extern const char CMD_SETUP_ALL[10];
extern const char CMD_SETUP_GIT[10];
extern const char CMD_SETUP_GITHUB[13];
extern const char CMD_SETUP_PROJECT[15];

#endif // _COMMANDS_H_
