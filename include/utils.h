#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

/*
 * Macros de utilidade compartilhada para mensagens, execução de comandos
 * e validação simples de alocação.
 */
#define MKDIR(name) mkdir(name, 0755)

#define CMD(command) \
    do { \
        int _status = system(command); \
        if (_status == -1) { \
            perror("system"); \
        } \
    } while (0)

#define CMDF(format, ...) \
    do { \
        int _size = snprintf(NULL, 0, format, ##__VA_ARGS__); \
        \
        if (_size < 0) { \
            fprintf(stderr, "Erro ao formatar comando\n"); \
            break; \
        } \
        \
        char* _command = malloc((size_t)_size + 1); \
        \
        if (_command == NULL) { \
            perror("malloc"); \
            break; \
        } \
        \
        snprintf(_command, (size_t)_size + 1, format, ##__VA_ARGS__); \
        \
        int _status = system(_command); \
        \
        if (_status == -1) { \
            perror("system"); \
        } \
        \
        free(_command); \
    } while (0)

#define INFO(msg) \
    printf("[INFO] " msg "\n")

#define WARN(msg) \
    printf("[WARN] " msg "\n")

#define PANIC(msg, ...) \
    printf("[PANIC] %s:%d (%s)" msg "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE)

#define ERROR(msg) \
    printf("[ERROR] " msg "\n")

#define SUCCESS(msg) \
    printf("[SUCCESS] " msg "\n")

#define TODO \
    printf("[TODO] "  " %s:%d  (%s)\n", __FILE__, __LINE__, __func__)

#define TODOF(msg, ...) \
    printf("[TODO] "  " %s:%d  (%s) {"msg"}\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LOG(fmt, ...) \
    printf("[%s] " fmt "\n", __func__, ##__VA_ARGS__)

#define FIXME(msg) \
    printf("[FIXME] %s:%d - " msg "\n", __FILE__, __LINE__)

#define NOTE(msg) \
    printf("[NOTE] %s:%d - "msg"\n", __FILE__, __LINE__)

#define BUG(msg) \
    printf("[BUG?] %s:%d - "msg"\n", __FILE__, __LINE__)

#define CHECK_ALLOC(...) \
    do { \
        void *ptrs[] = { __VA_ARGS__ }; \
        size_t n = sizeof(ptrs) / sizeof(ptrs[0]); \
        for (size_t i = 0; i < n; i++) { \
            if (ptrs[i] == NULL) { \
                perror("malloc"); \
                exit(EXIT_FAILURE); \
            } \
        } \
    } while (0)

#define SAVE_PATH "home/pms"

extern const char STANDART_MAIN_C[69];
char* readLine(void);

#endif // _UTILS_H_
