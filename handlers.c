#include <dirent.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
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

void exit_prog();
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

struct {
    char path[PATH_MAX];
    char name[PATH_MAX];
    enum {
        CUT,
        COPY,
        NOTHING,
    } mode;
} paste;

void cut() {
    strcpy(paste.path, path);
    paste.path[path_len] = '/';
    snprintf(paste.path + path_len + 1, PATH_MAX, "%s",
             files[cursor_pos]->d_name);
    paste.mode = CUT;
    strcpy(paste.name, files[cursor_pos]->d_name);
}

void copy() {
    strcpy(paste.path, path);
    paste.path[path_len] = '/';
    snprintf(paste.path + path_len + 1, PATH_MAX, "%s",
             files[cursor_pos]->d_name);
    paste.mode = COPY;
    strcpy(paste.name, files[cursor_pos]->d_name);
}

void c_paste() {
    if (paste.mode == NOTHING) {
        return;
    }
    if (access(paste.path, F_OK) != 0) {
        exit_prog();
        paste.mode = NOTHING;
        return;
    }
    int fd_out = open(paste.path, O_RDONLY);
    int old_len = path_len;
    path[path_len++] = '/';
    path[path_len] = '\0';
    path_len +=
        snprintf(path + path_len, PATH_MAX, "%s", paste.name);
    int fd_in = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    path_len = old_len;
    path[path_len] = '\0';
    if (fd_in == -1 || fd_out == -1) {
        exit_prog();
        paste.mode = NOTHING;
        close(fd_in);
        close(fd_out);
        scan_dir();
        print_dir();
        return;
    }
    int size = lseek(fd_out, 0, SEEK_END);
    lseek(fd_out, 0, SEEK_SET);
    if (ftruncate(fd_in, size) == -1) {
        exit_prog();
        paste.mode = NOTHING;
        close(fd_in);
        close(fd_out);
        scan_dir();
        print_dir();
        return;
    }
    void *buf = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_in, 0);
    if (buf == MAP_FAILED) {
        exit_prog();
        paste.mode = NOTHING;
        close(fd_in);
        close(fd_out);
        scan_dir();
        print_dir();
        return;
    }
    int readed = 0;
    while (1) {
        int cnt = read(fd_out, buf + readed, size);
        if (cnt <= 0) {
            break;
        }
        readed += cnt;
    }
    close(fd_in);
    close(fd_out);
    munmap(buf, size);
    if (paste.mode == CUT) {
        remove(paste.path);
    }
    paste.mode = NOTHING;
    int old_hidden_files = hidden_files;
    int old_cursor_pos = cursor_pos;
    scan_dir();
    hidden_files = old_hidden_files;
    cursor_pos = old_cursor_pos;
    print_dir();
}