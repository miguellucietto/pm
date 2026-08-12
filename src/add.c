#include <fenv.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>

#include "add.h"
#include "shared.h"


static char *path;
/* Flags accepted by `pm add`. */
//\\// ----------------------------------------------------------------//\\//
static bool pmpath = false; // Accept the path of the pm project // -p
static bool verbose = false; // Print the process               // -v
static bool force = false;   // Add file anywhere              // -f
static bool noc = false; // No .c file                        // -h
static bool noh = false; // No .h file                       // -c
//\\// ----------------------------------------------------------------//\\//

/* Create the source/header pair requested by the active flags. */
int add_file(const char *name) {
  if (!name) {
    ERROR("Provide a name to add");
    return EXIT_FAILURE;
  }

  FILE *f;

  // Verifying .pm
  if (!force) {
    if (pmpath) {
      char opath[strlen(path) + 10];
      
      snprintf(opath, sizeof(opath), "%s/.pm", path);
      f = fopen(opath, "r");
      if (!f) {
	ERROR("The path '%s' is not a pm project", path);
	return EXIT_FAILURE;
      }
    } else {
      f = fopen(".pm", "r");
      if (!f) {
	ERROR("The command has to be used in a pm project");
	return EXIT_FAILURE;
      }
    }
    fclose(f);    
  }
  

    // Making paths      ↓
    if (verbose) puts("Making paths...");    
    size_t s = strlen(name) + 1000;
    char fileh[s], filec[s];

    // Creating include path
    if (pmpath) {
      snprintf(fileh, sizeof(fileh), "%s/include/%s.h", path, name);
    } else {
      snprintf(fileh, sizeof(fileh), "include/%s.h", name);
    }
    if (verbose) printf("Path %s made\n", fileh);


    // Creating source path
    if (pmpath) {
      snprintf(filec, sizeof(filec), "%s/src/%s.c", path, name);
    } else {
      snprintf(filec, sizeof(filec), "src/%s.c", name);
    }
    if (verbose) printf("Path %s made\n", filec);
    

    if (verbose) puts("Creating files...");
    // Creating source file
    if (!noc) {
      f = fopen(filec, "w");
      if (f) {
	fprintf(f, "#include \"%s\"", fileh);
	fclose(f);
      } else {
	ERROR("Could not create '%s'", filec);
      }
    }

    // Creating header file
    if (!noh) { 
      f = fopen(fileh, "w");
      if (f) {
	fprintf(f, "#ifndef %s_H_\n#define %s_H_\n\n#endif /* %s_H_ */ \n", name, name, name);
	fclose(f);
      } else {
	ERROR("Could not create '%s'", fileh);
      }
    }
    
    if (verbose) puts("Finished process!");
    return EXIT_SUCCESS;
}


/* Parse `pm add` flags and create every non-flag file name. */
int pm_add(int argc, char **argv) {
  get_flags(argc, argv);
  if (has_flag('p')) {
    pmpath = true;
    path = argv[argc - 1];
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

  const char* flags = "phcvf";
  warn_invalid_flags(strlen(flags), flags);
  
  if (noh && noc) {
    if (verbose)
      printf("-c and -h?\nBRUUUUUUUHHHHHH\n\n");
    return EXIT_SUCCESS;
  }

  for (int i = 0; i < argc; i++) {
    if (verbose) printf("argv[%d] = %s\n", i, argv[i]);
    
    if (pmpath && i == argc - 1)
      continue;
    
    if (!is_flag(argv[i]) && add_file(argv[i]) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
