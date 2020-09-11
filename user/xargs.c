#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fs.h"

// debug-use
void print_argv(char** argv) {
    while (*argv) {
        printf("%s ", *argv);
        argv++;
    }
    printf("\n");
}

// 0 - EOF occurred
// 1 - successful get line
//-1 - overflow
int readline(char* buf, int n) {
    char* p = buf;
    int num = 0;
    while (n - (p - buf) > 0 && (num = read(0, p, n - (p - buf))) > 0) {
        p += num;
        if (*(p - 1) == '\n') {
            *(p - 1) = '\0';
            return 1;
        }
    }
    if (num == 0) {
        *p = '\0';
        return 0;
    } else {
        return -1;
    }
}

void runline(int argc, char** argv, char* line) {
    if (line == 0 || strlen(line) == 0) {
        return;
    }
    char* start = line;
    while (*start == ' ') {
        start++;
    }
    char* p = start;
    while (*p != '\0') {
        if (*p == ' ') {
            *p = '\0';
            argv[argc] = (char*)malloc((p - start + 1) * sizeof(char));
            strcpy(argv[argc], start);
            argc++;
            do {
                p++;
            } while (*p == ' ');
            start = p;
            continue;
        }
        p++;
    }
    if (start != p) {
        argv[argc] = (char*)malloc((p - start + 1) * sizeof(char));
        strcpy(argv[argc], start);
        argc++;
    }
    argv[argc] = 0;
    // print_argv(argv);
    if (fork() == 0) {
        exec(argv[0], argv);
    }
    wait(0);
}

int main(int argc, char const* argv[]) {
    char* exec_argv[MAXARG];
    exec_argv[0] = "echo";
    // the xargs -n 1 case
    if (argc >= 3 && argv[1][1] == 'n' && strlen(argv[1]) == 2 && argv[2][0] == '1' && strlen(argv[2]) == 1) {
        int i;
        for (i = 0; i < argc - 3; i++) {
            exec_argv[i] = (char*)malloc((strlen(argv[i + 3]) + 1) * sizeof(char*));
            strcpy(exec_argv[i], argv[i + 3]);
        }
        i = i > 0 ? i : 1;
        int r;
        char buf[DIRSIZ];
        while (1) {
            r = readline(buf, DIRSIZ);
            if (r > 0) {
                runline(i, exec_argv, buf);
            } else if (r == 0) {
                runline(i, exec_argv, buf);
                break;
            } else {
                fprintf(2, "xargs: line is too long\n");
                exit(1);
            }
        }
    } else {
        int i;
        for (i = 0; i < argc - 1; i++) {
            exec_argv[i] = (char*)malloc((strlen(argv[i + 1]) + 1) * sizeof(char*));
            strcpy(exec_argv[i], argv[i + 1]);
        }
        i = i > 0 ? i : 1;
        int r;
        char buf[DIRSIZ];
        buf[0] = '\0';
        while (1) {
            r = readline(buf + strlen(buf), DIRSIZ - strlen(buf));
            if (r > 0) {
                int len = strlen(buf);
                buf[len] = ' ';
                buf[len + 1] = '\0';
            } else if (r == 0) {
                runline(i, exec_argv, buf);
                break;
            } else {
                fprintf(2, "xargs: line is too long\n");
                exit(1);
            }
        }
    }
    exit(0);
}
