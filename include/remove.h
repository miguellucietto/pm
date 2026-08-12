#ifndef REMOVE_H
#define REMOVE_H

/* Remove the selected source/header files for one module name. */
int remove_file(const char*);

/* CLI handler for `pm rm`. */
int pm_remove(int argc, char** argv);

#endif
