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
int show_hidden_files = 0;

void exit_prog();
void scan_dir();
void print_dir();
void up();
void down();
void enter();
void delete();
void change_hide_mode();

int main() {
    getcwd(path, PATH_MAX);
    path_len = strlen(path);

    initscr();
    raw();
    noecho();

    term_width = getmaxx(stdscr);
    term_height = getmaxy(stdscr);

    start_color();
    // init_pair(1, COLOR_WHITE, COLOR_BLACK);
    // init_pair(2, COLOR_CYAN, COLOR_BLACK);
    // init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    // init_pair(4, COLOR_BLUE, COLOR_BLACK);
    // init_pair(5, COLOR_WHITE, COLOR_GREEN);
    // init_pair(6, COLOR_CYAN, COLOR_GREEN);
    // init_pair(7, COLOR_MAGENTA, COLOR_GREEN);
    // init_pair(8, COLOR_BLUE, COLOR_GREEN);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLUE);
    init_pair(6, COLOR_CYAN, COLOR_BLUE);
    init_pair(7, COLOR_YELLOW, COLOR_BLUE);
    init_pair(8, COLOR_GREEN, COLOR_BLUE);

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
        if (c == 'h') {
            change_hide_mode();
        }
    }

    exit_prog();
}