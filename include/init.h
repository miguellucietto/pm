#ifndef INIT_H
#define INIT_H

/* Create one project named root using the flags selected by pm_init(). */
int init(char* root);

/* CLI handler for `pm init`. */
int pm_init(int argc, char **argv);

/* Replace a text file with content. */
int write_file(const char* p, const char* content);

/* Write the enabled default project files below r. */
int write_default_files(const char* r);


#endif // INIT_H
