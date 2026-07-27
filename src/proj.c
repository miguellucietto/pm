#include "global.h"

// TODO: fazer variações de projetos

void create_project(char* title)
{
    if (!title) return;

    MKDIR(title);
    
    char path[256];

    snprintf(path, sizeof(path), "%s/src", title);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/src/main.c", title);
    FILE* main = fopen(path, "w");
    fputs(STANDART_MAIN_C, main);
    fclose(main);

    snprintf(path, sizeof(path), "%s/include", title);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/build", title);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/bin", title);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/Makefile", title);
    FILE* makefile = fopen(path, "w");
    fputs(STANDART_MAKEFILE_CONTENT, makefile);
    fclose(makefile);
}