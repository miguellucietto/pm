#include <stdio.h>
#include <string.h>
#include "add.h"

// TODO: Adicionar template -p para adicionar path e não precisar estar em um projeto com .pm

void pm_add(const char* name) {
  if (!name) {
    fprintf(stderr, "Provide a name to add\n%s", __func__);
    return;
  }
  
  FILE* f = fopen(".pm", "r");
  if (!f) {
    printf("[INFO] Cannot add file outside of a pm project yet\n");
    return;
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

  
  
  
}
