#include "utils.h"
#include "commands.h"

/*
 * Definições compartilhadas de utilidades e constantes de comando.
 * Este arquivo concentra o que antes estava misturado em headers,
 * deixando a organização mais profissional sem alterar o comportamento.
 */

const char STANDART_MAIN_C[69] = "#include <stdio.h>\n\nint main(int argc, char** argv)\n{\n\t\n\treturn 0;\n}";

const char CMD_LIST_PROJECT[14] = "list project";
const char CMD_INFO_PROJECT[14] = "info project";
const char CMD_CREATE_PROJECT[16] = "create project";
const char CMD_LOCATE_PROJECT[16] = "locate project";
const char CMD_DELETE_PROJECT[16] = "delete project";

const char CMD_SAVE_GIT[9] = "save git";
const char CMD_START_GIT[10] = "start git";

const char CMD_SETUP_ALL[10] = "setup all";
const char CMD_SETUP_GIT[10] = "setup git";
const char CMD_SETUP_GITHUB[13] = "setup github";
const char CMD_SETUP_PROJECT[15] = "setup project";

char* readLine(void)
{
    int cap = 4;
    char* line = malloc(cap);
    CHECK_ALLOC(line);

    int count = 0;
    int c = 0;

    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (count + 1 >= cap)
        {
            char* tmp = realloc(line, cap * 2);
            CHECK_ALLOC(tmp);
            line = tmp;
            cap *= 2;
        }
        line[count++] = c;
    }
    line[count] = '\0';

    return line;
}
