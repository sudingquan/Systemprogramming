/*************************************************************************
	> File Name: terminal_2.0.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 六  4/27 18:09:16 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

//获得当前用户的用户名
char *getUsername() {
    struct passwd *usr = getpwuid(getuid());

    return usr->pw_name;
}

//获得当前工作目录
char *getPwd() {
    char *pwd;
    getcwd(pwd, 100);
    return pwd;
}

int main(int argc, char *argv[]) {
    char *hostname = malloc(sizeof(char) * 100);
    gethostname(hostname, sizeof(hostname));
    printf("%s\n", getUsername);
    //printf("%s@%s:%s $ \n", getUsername, hostname, getPwd());
    return 0;
}
