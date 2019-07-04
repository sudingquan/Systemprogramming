/*************************************************************************
	> File Name: fork.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 日  5/26 17:09:35 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int x = 0;
    for (int i = 0; i < 9; i++) {
        pid=fork();
        x = i++;
        if (pid == 0) break;
    }
    if (pid != 0) x++;
    printf("%d\n", x);
    fflush(stdout);
    sleep(10000);
    return 0;
}
