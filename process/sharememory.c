/*************************************************************************
	> File Name: sharememory.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 日  7/28 18:33:55 2019
 ************************************************************************/

#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>

int main() {
    key_t key;
    pid_t pid;
    char *share_memory = NULL;
    key = ftok(".", 1);
    if (key < -1) {
        perror("ftok");
        exit(1);
    }
    int shmid;
    shmid = shmget(key, 4096, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    share_memory = shmat(shmid, 0, 0);
    if (share_memory < 0) {
        perror("shmat");
        exit(1);
    }
    pid = fork();
    if (pid == 0) {
        while (1) {
            memset(share_memory, 0, 4096);
            scanf("%s", share_memory);
        }
    }
    fd_set set;
    FD_ZERO(&set);
    FD_SET(0, &set);
    int ret;
    while (1) {
        fflush(stdout);
        ret = select(1, &set, NULL, NULL, NULL);
        if (ret > 0) {
            printf("father process -> %s\n", share_memory);
            memset(share_memory, 0, 4096);
        }
    }
    return 0;
}
