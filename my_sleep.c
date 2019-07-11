/*************************************************************************
	> File Name: my_sleep.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/11 16:17:15 2019
 ************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#define SECOND 2

void f() {

}

void my_sleep(int second) {
    signal(SIGALRM, f);
    alarm(second);
    pause();
}

int main() {
    my_sleep(SECOND);
    return 0;
}
