#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "add.h"
#include "shared.h"


//\\// ----------------------------------------------------------------//\\//
bool pmpath = false; // Accept the path of the pm project // -p
bool verbose = false; // Print the process               // -v
bool force = false;   // Add file anywhere              // -f
bool noc = false; // No .c file                        // -h
bool noh = false; // No .h file                       // -c
//\\// ----------------------------------------------------------------//\\//

int add_file(const char *name) {
  if (!name) {
    fprintf(stderr, "Provide a name to add\n%s", __func__);
    return EXIT_FAILURE;
  }

  FILE *f;
  
  if (!force) {
    f = fopen(".pm", "r");
    if (!f) {
      printf("[INFO] Cannot add file outside of a pm project yet\n");
      return EXIT_FAILURE;
    }
    
    
    fclose(f);
  }
  
    if (verbose) puts("Making paths...");    
    size_t s = strlen(name) + 100;
    char fileh[s], filec[s];
    snprintf(fileh, sizeof(fileh), "include/%s.h", name);
    if (verbose) printf("Path %s made\n", fileh);

    snprintf(filec, sizeof(filec), "src/%s.c", name);
    if (verbose) printf("Path %s made\n", filec);
    

    if (verbose) puts("Creating files...");

    if (!noc) {
      f = fopen(filec, "w");
      if (f) {
	fprintf(f, "#include \"%s\"", fileh);
	fclose(f);
      } else {
	fprintf(stderr, "Could not create %s\n", filec);
      }
    }

    if (!noh) { 
      f = fopen(fileh, "w");
      if (f) {
	fputs("#ifndef\n#define\n\n#endif", f);
	fclose(f);
      } else {
	fprintf(stderr, "Could not create %s\n", fileh);
      }
    }
    
    if (verbose) puts("Finished process!");
    return EXIT_SUCCESS;
}


int pm_add(int argc, char **argv) {
  get_flags(argc, argv);
  if (has_flag('p')) {
    pmpath = true;
  }
  if (has_flag('v')) {
    verbose = true;
  }
  if (has_flag('h')) {
    noh = true;
  }
  if (has_flag('c')) {
    noc = true;
  }
  if (has_flag('f')) {
    force = true;
  }

  if (noh && noc) {
    if (verbose)
      printf("\nBRUUUUUUUHHHHHH\n");
    return EXIT_SUCCESS;
  }
  
  for (int i = 0; i < argc; i++) {
    if (!is_flag(argv[i]) && add_file(argv[i]) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
