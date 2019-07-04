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
    flock(fp->_sfileno(fp), LOCK_EX);
    return 0;
}
