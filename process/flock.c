/*************************************************************************
	> File Name: flock.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/ 4 20:35:00 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#define PATH "/tmp/lock"


int main() {
    FILE *fp = fopen(PATH, "w");
    if (fp == NULL) {
        perror("fopen");
    }
    pid_t pid;
    flock(fileno(fp), LOCK_EX);
    printf("已上锁\n");
    int n = 10;
    while (n--) {
        sleep(1);
        printf("%d\n", n);
    }
    return 0;
}
