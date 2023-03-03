#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void exit_prog();

void update_buffer(int new_size);

int comp(const void *a, const void *b);

void scan_dir();

void print_dir();