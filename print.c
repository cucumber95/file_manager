#include "print.h"

extern char path[PATH_MAX];
extern int path_len;
extern int term_width;
extern int term_height;
extern int cursor_pos;
extern int hidden_files;
extern int dirent_size;
extern int buffer_size;
extern struct dirent **files;
extern int show_hidden_files;

void exit_prog() {
    endwin();
    free(files);
    _exit(0);
}

void update_buffer(int new_size) {
    struct dirent **new_files = realloc(files, new_size * sizeof(*new_files));
    if (new_files == NULL) {
        free(new_files);
        exit_prog();
    }
    files = new_files;
    buffer_size = new_size;
}

int comp(const void *a, const void *b) {
    if (strcmp((*(struct dirent **)(a))->d_name, ".") == 0) {
        return 1;
    }
    if (strcmp((*(struct dirent **)(b))->d_name, ".") == 0) {
        return -1;
    }
    if (strcmp((*(struct dirent **)(a))->d_name, "..") == 0) {
        return -1;
    }
    if (strcmp((*(struct dirent **)(b))->d_name, "..") == 0) {
        return 1;
    }
    return strcmp((*(struct dirent **)(a))->d_name,
                  (*(struct dirent **)(b))->d_name);
}

void scan_dir() {
    hidden_files = 0;
    cursor_pos = 0;
    DIR *dir = opendir(path);
    struct dirent *cur_file;
    int cnt = 0;
    while ((cur_file = readdir(dir)) != NULL) {
        if (strcmp(cur_file->d_name, ".") == 0) {
            continue;
        }
        if (!show_hidden_files && cur_file->d_name[0] == '.' && strcmp(cur_file->d_name, "..") != 0) {
            continue;
        }
        cnt++;
    }
    if (cnt > buffer_size) {
        update_buffer(cnt);
    }
    closedir(dir);
    dir = opendir(path);
    int i = 0;
    while ((cur_file = readdir(dir)) != NULL) {
        if (strcmp(cur_file->d_name, ".") == 0) {
            continue;
        }
        if (!show_hidden_files && cur_file->d_name[0] == '.' && strcmp(cur_file->d_name, "..") != 0) {
            continue;
        }
        files[i++] = cur_file;
    }
    qsort(files, cnt, sizeof(*files), comp);
    dirent_size = cnt;
    closedir(dir);
}

void print_dir() {
    bkgdset(COLOR_PAIR(1));
    if (show_hidden_files) {
        bkgdset(COLOR_PAIR(5));
    }
    clear();
    for (int i = hidden_files;
         i < hidden_files + term_height && i < dirent_size; ++i) {
        if (i == cursor_pos) {
            move(i - hidden_files, 0);
            if (files[i]->d_type == DT_DIR) {
                attrset(COLOR_PAIR(2));
                if (show_hidden_files) {
                    attrset(COLOR_PAIR(6));
                }
                printw("-> ");
            } else {
                attrset(COLOR_PAIR(1));
                if (show_hidden_files) {
                    attrset(COLOR_PAIR(5));
                }
                printw("-> ");
            }
        }
        move(i - hidden_files, 3);
        if (files[i]->d_type == DT_DIR) {
            attrset(COLOR_PAIR(2));
            if (show_hidden_files) {
                attrset(COLOR_PAIR(6));
            }
            printw("%s", files[i]->d_name);
        } else {
            attrset(COLOR_PAIR(1));
            if (show_hidden_files) {
                attrset(COLOR_PAIR(5));
            }
            printw("%s", files[i]->d_name);
        }
        move(i - hidden_files, term_width - 12);
        if (files[i]->d_type == DT_DIR) {
            attrset(COLOR_PAIR(4));
            if (show_hidden_files) {
                attrset(COLOR_PAIR(8));
            }
            printw("DIR");
        } else {
            int old_len = path_len;
            path[path_len++] = '/';
            path[path_len] = '\0';
            path_len +=
                snprintf(path + path_len, PATH_MAX, "%s", files[i]->d_name);
            struct stat buf;
            if (stat(path, &buf) != -1) {
                attrset(COLOR_PAIR(3));
                if (show_hidden_files) {
                    attrset(COLOR_PAIR(7));
                }
                printw("%ld", buf.st_size);
            }
            path_len = old_len;
            path[path_len] = '\0';
        }
    }
    refresh();
}