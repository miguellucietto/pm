#ifndef _GLOBAL_H__
#define _GLOBAL_H__

#include "utils.h"
#include "commands.h"

/*
 * Header central para compatibilidade do projeto.
 * Mantém a inclusão das definições compartilhadas sem misturar
 * utilidades, constantes de comando e protótipos de funções.
 */
#define IncludeEveryThing \
#include <stdio.h>\
#include <stdlib.h>\
#include <stdbool.h>\
#include <string.h>\
#include "global.h"\
#include "utils.h"\
#include "git.h"\
#include "pdir.h"\
#include "proj.h"\
#include "cmd.h"\
#include "commands.h"\


#endif // _GLOBAL_H__