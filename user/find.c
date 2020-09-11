#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

static void match(const char* target, char* root) {
    int i, j = strlen(root);
    for (i = strlen(root) - 1; i >= 0 && root[i] != '/'; i--) {
        if (root[i] == '.') {
            j = i;
        }
    }
    char name[DIRSIZ + 1];
    strcpy(name, root + i + 1);
    if (j > i + 1) {
    }
    int len = j > i + 1 ? j - i - 1 : strlen(name);
    for (i = 0; i < len && name[i] == target[i]; i++)
        ;
    if (i == len) {
        printf("%s\n", root);
    }
}

void find(const char* target, char* root) {
    // printf("finding %s in %s\n", target, root);
    match(target, root);
    int fd;
    struct dirent dir;
    struct stat st;
    if ((fd = open(root, O_RDONLY)) < 0) {
        fprintf(2, "%d find: cannot open %s\n", fd, root);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", root);
        close(fd);
        return;
    }
    if (st.type == T_DIR) {
        char buf[512];
        strcpy(buf, root);
        char* p = buf + strlen(root);
        *p = '/';
        p++;
        while (read(fd, &dir, sizeof(struct dirent)) == sizeof(struct dirent)) {
            if (strlen(root) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                fprintf(2, "find: path too long\n");
            }
            if (strlen(dir.name) == 0 || dir.name[0] == '.') {
                continue;
            }
            strcpy(p, dir.name);
            // printf("cat %s to %s\n", dir.name, buf);
            find(target, buf);
        }
    }
    close(fd);
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: find file\n");
    }
    const char* target = argv[1];
    find(target, ".");
    exit(0);
}
