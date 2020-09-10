#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    int pipe_fds[2];
    pipe(pipe_fds);
    int pid;
    char ph;
    if ((pid = fork()) == 0) {
        read(pipe_fds[0], &ph, 1);
        printf("%d: received ping\n", getpid());
        write(pipe_fds[1], "\n", 1);
        exit(0);
    }
    write(pipe_fds[1], "\n", 1);
    read(pipe_fds[0], &ph, 1);
    printf("%d: received pong\n", getpid());
    exit(0);
}
