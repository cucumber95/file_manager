#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern char path[PATH_MAX];
extern int path_len;
extern int term_width;
extern int term_height;
extern int cursor_pos;
extern int hidden_files;
extern int dirent_size;
extern struct dirent **files;
extern int show_hidden_files;

void scan_dir();
void print_dir();

void up() {
    if (cursor_pos == 0) {
        return;
    }
    cursor_pos--;
    if (cursor_pos < hidden_files) {
        hidden_files--;
    }
    print_dir();
}

void down() {
    if (cursor_pos == dirent_size - 1) {
        return;
    }
    cursor_pos++;
    if (cursor_pos - hidden_files == term_height) {
        hidden_files++;
    }
    print_dir();
}

void enter() {
    if (files[cursor_pos]->d_type == DT_DIR) {
        if (strcmp(files[cursor_pos]->d_name, "..") != 0) {
            path[path_len++] = '/';
            path[path_len] = '\0';
            path_len += snprintf(path + path_len, PATH_MAX, "%s",
                                 files[cursor_pos]->d_name);
            scan_dir();
            print_dir();
        } else {
            while (path[path_len] != '/') {
                path_len--;
            }
            path[path_len] = '\0';
            scan_dir();
            print_dir();
        }
    }
}

void delete () {
    int old_len = path_len;
    path[path_len++] = '/';
    path[path_len] = '\0';
    path_len +=
        snprintf(path + path_len, PATH_MAX, "%s", files[cursor_pos]->d_name);
    if (remove(path) == 0) {
        path_len = old_len;
        path[path_len] = '\0';
        int old_cursor_pos = cursor_pos, old_hidden_files = hidden_files;
        scan_dir();
        cursor_pos = old_cursor_pos;
        hidden_files = old_hidden_files;
        if (cursor_pos == dirent_size) {
            cursor_pos--;
            if (cursor_pos < hidden_files) {
                hidden_files--;
            }
        }
        print_dir();
    } else {
        path_len = old_len;
        path[path_len] = '\0';
    }
}

void change_hide_mode() {
    show_hidden_files ^= 1;
    scan_dir();
    print_dir();
}