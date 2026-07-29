#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "pdir.h"

/*
 * Implementação das operações de criação e localização de projetos.
 * O fluxo foi preservado, apenas ficou mais claro pela separação temática.
 */


void setup_project(void)
{
    char** con = calloc(5, sizeof(char*));
    CHECK_ALLOC(con);
    for (int i = 0; i < 5; i++)
    {
        con[i] = malloc(64);
        CHECK_ALLOC(con[i]);
    }

    printf("==============================\n");
    printf("Project Directory Setup\n");
    printf("==============================\n");

    printf("\nCompiler: ");
    con[0] = readLine();

    printf("\nSource directory name: ");
    con[1] = readLine();

    printf("\nInclude directory name: ");
    con[2] = readLine();

    printf("\nBuild directory name: ");
    con[3] = readLine();

    printf("\nBinary directory name: ");
    con[4] = readLine();

    save_pdir_content(con);
}


void create_project(char* title)
{
    if (!title) return;

    MKDIR(title);
    
    char path[256];
    char** pdir = get_pdir_content();

    snprintf(path, sizeof(path), "%s/%s", title, pdir[0]);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/%s/main.c", title, pdir[0]);
    FILE* main = fopen(path, "w");
    fputs(STANDART_MAIN_C, main);
    fclose(main);

    snprintf(path, sizeof(path), "%s/%s", title, pdir[1]);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/%s", title, pdir[2]);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/%s", title, pdir[3]);
    MKDIR(path);

    snprintf(path, sizeof(path), "%s/Makefile", title);
    FILE* makefile = fopen(path, "w");
    fputs(get_makefile_content(), makefile);
    fclose(makefile);
}

void locate_project(char* name)
{
    CHECK_ALLOC(name);
    CMDF("find %s", name);
    exit(0);
}

