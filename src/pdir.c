#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "pdir.h"

/*
 * Implementação relacionada à configuração do diretório do projeto.
 * O conteúdo foi mantido intacto, porém o arquivo agora está mais bem
 * caracterizado pela sua responsabilidade.
 */

void save_pdir_content(char** con)
{
    if (!con || !*con) {
        PANIC("could not find content enough");
    }
    FILE* file = fopen(SAVE_PATH"/makefile.txt", "w");
    if (!file) {
        ERROR("could not open " SAVE_PATH"/makefile.txt");
        exit(0);
    }
    fputs("y\n", file);

    for (int i = 0; i < 5; i++) // 5: CC, SRC, INCLUDE, BUILD and BIN
    {
        fputs(con[i], file);
        fputc('\n', file);
    }

    fclose(file);
}

char** get_pdir_content(void)
{
    FILE* file = fopen(SAVE_PATH"/makefile.txt", "r");
    if (!file) {
        ERROR("could not open " SAVE_PATH"/makefile.txt");
        exit(0);
    }
    if (fgetc(file) == 'n') {
        INFO("setup makefile content first");
        exit(0);
    }
    char line[128];
    fgets(line, sizeof(line), file);
    char* SRC = NULL;
    char* INCLUDE = NULL;
    char* CC = NULL;
    char* BIN = NULL;
    char* BUILD = NULL;
    CC = strtok(line, "\n");
    SRC = strtok(NULL, "\n");
    INCLUDE = strtok(NULL, "\n");
    BUILD = strtok(NULL, "\n");
    BIN = strtok(NULL, "\n");
    fclose(file);

    char** pdir = calloc(4, sizeof(char*));
    CHECK_ALLOC(pdir);
    for (int i = 0; i < 4; i++)
    {
        pdir[i] = malloc(64);
        CHECK_ALLOC(pdir[i]);
    }

    strcpy(pdir[0], SRC);
    strcpy(pdir[1], INCLUDE);
    strcpy(pdir[2], BUILD);
    strcpy(pdir[3], BIN);

    (void)CC;
    return pdir;
}

const char* get_makefile_content(void)
{
    FILE* file = fopen(SAVE_PATH"/makefile.txt", "r");
    if (!file) {
        ERROR("could not open " SAVE_PATH"/makefile.txt");
        exit(0);
    }
    if (fgetc(file) == 'n') {
        INFO("setup makefile content first");
        exit(0);
    }
    // get the info to make the content
    char line[128];
    fgets(line, sizeof(line), file);
    char *SRC, *INCLUDE, *CC, *BIN, *BUILD;
    CC = strtok(line, "\n");
    SRC = strtok(NULL, "\n");
    INCLUDE = strtok(NULL, "\n");
    BUILD = strtok(NULL, "\n");
    BIN = strtok(NULL, "\n");
    fclose(file);

    // makes the Makefile's content and return it
    char con[10000];
    snprintf(con, sizeof(con), "CC = %s\n"
    "\n"
    "INCLUDE_DIR = %s\n"
    "SRC_DIR = %s\n"
    "BUILD_DIR = %s\n"
    "BIN_DIR = %s\n"
    "\n"
    "TARGET = $(BIN_DIR)/all\n"
    "\n"
    "CFLAGS = -Wall -Wextra -std=c17 -ggdb\n"
    "DEPFLAGS = -MMD -MP\n"
    "CPPFLAGS = -I$(INCLUDE_DIR)\n"
    "\n"
    "CFILES = $(wildcard $(SRC_DIR)/*.c)\n"
    "OFILES = $(patsubst $(SRC_DIR)/%%.c,$(BUILD_DIR)/%%.o,$(CFILES))\n"
    "\n"
    ".PHONY: all all-run info dirs clean run\n"
    "\n"
    "all: dirs $(TARGET)\n"
    "\n"
    "$(TARGET): $(OFILES)\n"
    "\t@$(CC) $(OFILES) $(CFLAGS) -o $(TARGET)\n"
    "\n"
    "all-run: all run\n"
    "\n"
    "dirs:\n"
    "\t@mkdir -p $(BIN_DIR) $(BUILD_DIR) $(SRC_DIR) $(INCLUDE_DIR)\n"
    "\t@mv $(wildcard *.c) $(SRC_DIR)/ 2>/dev/null || true\n"
    "\t@mv $(wildcard *.h) $(INCLUDE_DIR)/ 2>/dev/null || true\n"
    "\n"
    "run: all\n"
    "\t@./$(TARGET)\n"
    "\n"
    "info:\n"
    "\t@echo \"=========================\"\n"
    "\t@echo \"          INFO           \"\n"
    "\t@echo \"=========================\"\n"
    "\t@echo\n"
    "\t@echo \"CFLAGS:\"\n"
    "\t@echo $(CFLAGS)\n"
    "\t@echo\n"
    "\t@echo \"DEPFLAGS:\"\n"
    "\t@echo $(DEPFLAGS)\n"
    "\t@echo\n"
    "\t@echo \"CPPFLAGS:\"\n"
    "\t@echo $(CPPFLAGS)\n"
    "\t@echo\n"
    "\t@echo \"FILES:\"\n"
    "\t@echo $(CFILES)\n"
    "\n"
    "clean:\n"
    "\t@rm -rf $(BIN_DIR) $(BUILD_DIR)\n"
    "\n"
    "$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.c\n"
    "\t@$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@\n"
    "\n"
    "-include $(OFILES:.o=.d)\n", CC, INCLUDE, SRC, BUILD, BIN);

    return con;
}