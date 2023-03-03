#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern char path[PATH_MAX];
extern int path_len;
extern int term_width;
extern int term_height;
extern int cursor_pos;
extern int hidden_files;
extern int dirent_size;
extern int buffer_size;
extern struct dirent **files;

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
        cnt++;
    }
    if (cnt > buffer_size) {
        update_buffer(cnt);
    }
    closedir(dir);
    dir = opendir(path);
    for (int i = 0; i < cnt; ++i) {
        cur_file = readdir(dir);
        files[i] = cur_file;
    }
    qsort(files, cnt, sizeof(*files), comp);
    dirent_size = cnt - 1;
    closedir(dir);
}

void print_dir() {
    clear();
    for (int i = hidden_files;
         i < hidden_files + term_height && i < dirent_size; ++i) {
        if (i == cursor_pos) {
            move(i - hidden_files, 0);
            if (files[i]->d_type == DT_DIR) {
                attron(COLOR_PAIR(1));
                printw("-> ");
                attroff(COLOR_PAIR(1));
            } else {
                printw("-> ");
            }
        }
        move(i - hidden_files, 3);
        if (files[i]->d_type == DT_DIR) {
            attron(COLOR_PAIR(1));
            printw("%s", files[i]->d_name);
            attroff(COLOR_PAIR(1));
        } else {
            printw("%s", files[i]->d_name);
        }
        move(i - hidden_files, term_width - 12);
        if (files[i]->d_type == DT_DIR) {
            attron(COLOR_PAIR(3));
            printw("DIR");
            attroff(COLOR_PAIR(3));
        } else {
            int old_len = path_len;
            path[path_len++] = '/';
            path[path_len] = '\0';
            path_len +=
                snprintf(path + path_len, PATH_MAX, "%s", files[i]->d_name);
            struct stat buf;
            if (stat(path, &buf) != -1) {
                attron(COLOR_PAIR(2));
                printw("%ld", buf.st_size);
                attroff(COLOR_PAIR(2));
            }
            path_len = old_len;
            path[path_len] = '\0';
        }
    }
    refresh();
}