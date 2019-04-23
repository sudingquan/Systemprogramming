/*************************************************************************
	> File Name: homework.c
	> Author: sudingquan 
	> Mail: 1151015256@qq.com 
	> Created Time: 日  4/14 19:32:36 2019
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

int main(int argc,char **argv)
{
    char *username;
    char *path;
    char hostname[65];
    username = getenv("LOGNAME");
    gethostname(hostname, sizeof(hostname));
    path = getenv("PWD");

    printf("\033[32m  %s@%s\n \033[0m",username,hostname);
    printf("\033[31m pwd = %s\n \033[0m",path);
    return 0;
}
