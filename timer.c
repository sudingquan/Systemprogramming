/*************************************************************************
	> File Name: timer.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/11 19:22:58 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

struct itimerval timer;

void wakeup() {
    printf("wakeup\n");
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec += 20000;
    setitimer(ITIMER_REAL, &timer, NULL);
}

int main() {
    signal(SIGALRM, wakeup);
    timer.it_value.tv_sec = 2;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 2;
    timer.it_interval.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        printf("Set timer failed!\n");
    }
    for(; ; ) {}
    return 0;
}
