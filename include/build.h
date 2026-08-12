#ifndef BUILD_H
#define BUILD_H

/* Entry point for `pm build`. */
int pm_build(int argc, char **argv);

/* Build the pm project located at path. */
int build_project(const char *path, int verbose);

#endif // BUILD_H
