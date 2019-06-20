/*************************************************************************
	> File Name: pthread.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 三  6/19 22:07:14 2019
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>

void *print(void *a) {
    printf("pthread start\n");
    printf("%d\n", *(int *)a);
    return NULL;
}

int main() {
    int height = 173;
    int weight = 145;
    int *x = &height;
    pthread_t pt1;
    pthread_t pt2;
    if (pthread_create(&pt1, NULL, print, (void *)x)) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_join(pt1, NULL)) {
        perror("pthread_join");
        exit(1);
    }
    if (pthread_create(&pt2, NULL, print, (void *)&weight)) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_join(pt2, NULL)) {
        perror("pthread_join");
        exit(1);
    }
    return 0;
}
