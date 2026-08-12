#ifndef ADD_H
#define ADD_H

/* CLI handler for `pm add`. */
int pm_add(int argc, char** argv);

/* Create the selected source/header files for one module name. */
int add_file(const char*);

#endif // ADD_H
