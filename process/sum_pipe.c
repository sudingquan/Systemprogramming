/*************************************************************************
	> File Name: sum_pipe.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/ 4 18:06:58 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int fd[2];
    int x = 0;
    long long sum = 0;
    if (pipe(fd) < 0) {
        printf("Create pipe error\n");
        exit(1);
    }
    for (int i = 0; i < 10; i++) {
        pid = fork();
        if (pid == 0) {
            x = i;
            break;
        }
    }
    if (pid == 0) {
        close(fd[0]);
        for (int i = 1; i <= 100000; i++) {
            sum += i + x * 100000;
        }
        printf("child process %d : %lld\n", x, sum);
        write(fd[1], &sum, sizeof(sum));
        close(fd[1]);
    } else if(pid > 0) {
        close(fd[1]);
        for (int i = 1; i <= 10; i++) {
            long long p;
            read(fd[0], &p, sizeof(p));
            sum += p;
            printf("main process %d loop : %lld\n", i, sum);
        }
        close(fd[0]);
    }
    return 0;
}
