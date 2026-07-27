#ifndef _GLOBAL_H__
#define _GLOBAL_H__

#include <stdbool.h>

// Utils
#define MKDIR(name) mkdir(name, 0755)


// Const variables
const char* STANDART_MAIN_C = "#include <stdio.h>\n\nint main(int argc, char** argv)\n{\n\treturn 0;\n}";
const char* CMD_MAKEFILE_CONTENT =
"CC = gcc\n"
"\n"
"INCLUDE_DIR = include\n"
"SRC_DIR = src\n"
"BUILD_DIR = build\n"
"BIN_DIR = bin\n"
"\n"
"TARGET = $(BIN_DIR)/all\n"
"\n"
"CFLAGS = -Wall -Wextra -std=c17 -ggdb\n"
"DEPFLAGS = -MMD -MP\n"
"CPPFLAGS = -I$(INCLUDE_DIR)\n"
"\n"
"CFILES = $(wildcard $(SRC_DIR)/*.c)\n"
"OFILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CFILES))\n"
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
"$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c\n"
"\t@$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@\n"
"\n"
"-include $(OFILES:.o=.d)\n";

// Structs
typedef struct
{
    char* cmd;
    bool arg;

    int count;
    char** args;
} CMD;

// Parsed commands
const char CMD_CREATE_PROJECT[15] = "create project";
const char CMD_INFO_PROJECT[13]   = "info project";
const char CMD_LOCATE_PROJECT[15] = "locate project";



#endif // _GLOBAL_H__