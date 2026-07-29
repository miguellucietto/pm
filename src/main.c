#include "cmd.h"

/*
 * Ponto de entrada principal do projeto.
 * O arquivo fica responsável apenas pela inicialização do fluxo.
 */

int main(int argc, char** argv)
{
    if (argc > 1) exec_cmd(get_cmd(argc, argv));

    return 0;
}