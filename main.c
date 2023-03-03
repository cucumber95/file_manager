#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char path[PATH_MAX];
int path_len = 0;
int term_width = 0;
int term_height = 0;
int hidden_files = 0;
int cursor_pos = 0;
int dirent_size = 0;
int buffer_size = 0;
struct dirent **files = NULL;

void exit_prog();
void scan_dir();
void print_dir();
void up();
void down();
void enter();
void delete ();

int main() {
    getcwd(path, PATH_MAX);
    path_len = strlen(path);

    initscr();
    raw();
    noecho();

    term_width = getmaxx(stdscr);
    term_height = getmaxy(stdscr);

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);

    scan_dir();
    print_dir();

    keypad(stdscr, TRUE);

    int c;
    while ((c = getch()) != 'q') {
        if (c == KEY_UP) {
            up();
        }
        if (c == KEY_DOWN) {
            down();
        }
        if (c == KEY_ENTER || c == '\n') {
            enter();
        }
        if (c == 'd') {
            delete ();
        }
    }
    
    exit_prog();
}