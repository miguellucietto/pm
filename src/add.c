#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "add.h"
#include "shared.h"

// TODO: Adicionar template -p para adicionar path e não precisar estar em um projeto com .pm


int add_file(const char* name) {
  if (!name) {
    fprintf(stderr, "Provide a name to add\n%s", __func__);
    return EXIT_FAILURE;
  }
  
  FILE* f = fopen(".pm", "r");
  if (!f) {
    printf("[INFO] Cannot add file outside of a pm project yet\n");
    return EXIT_FAILURE;
  }
   
    fclose(f);
    
    size_t s = strlen(name) + 100;
    char fileh[s], filec[s];
    snprintf(fileh, sizeof(fileh), "include/%s.h", name);
    snprintf(filec, sizeof(filec), "src/%s.c", name);

    f = fopen(filec, "w");
    if (f) {
      fprintf(f, "#include \"%s\"", fileh);
      fclose(f); 
    }
    f = fopen(fileh, "w");
    if (f) {
      fputs("#ifndef // FILE_NAME_H\n#define // FILE_NAME_H\n\n#endif", f);
      fclose(f);
    }
    return EXIT_SUCCESS;
}


int pm_add(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    if (add_file(argv[i]) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
