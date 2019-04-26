/*************************************************************************
	> File Name: example.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 五  4/26 13:46:21 2019
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

void parsecmd(char *s) {
    if (!strncasecmp(s, "cd", 2)) {
        if (strlen(s) == 2) {
            struct passwd *user;
            int uid = getuid();
            user = getpwuid(uid);
            chdir(user->pw_dir);
        } else if (chdir(s + 3) != 0) {
            printf("-bash: cd: %s: %s\n", s + 3, strerror(errno));
        }
    } else if (!strncasecmp(s, "exit", 4)) {
        exit(0);
    }
}
int main() {
    struct passwd *user;
    char hostname[65];
    int uid = getuid();
    char pwd[105];
    char cmd[1005];
    user = getpwuid(uid);
    gethostname(hostname, sizeof(hostname));
    signal(SIGINT,SIG_IGN);
    while (1) {
        int homelenth = strlen(user->pw_dir);
        getcwd(pwd, 100);
        if (!strcmp(user->pw_name, "root")) {
            if (!strncasecmp(getcwd(pwd, 100), user->pw_dir, homelenth)) {
                printf("\033[32m%s@%s\033[0m" ":" "\033[31m~%s\033[0m# ", user->pw_name, hostname, pwd + homelenth);
            } else {
                printf("\033[32m%s@%s\033[0m" ":" "\033[31m%s\033[0m# ", user->pw_name, hostname, pwd);
            }
        } else {
            if (!strncasecmp(getcwd(pwd, 100), user->pw_dir, homelenth)) {
                printf("\033[32m%s@%s\033[0m" ":" "\033[31m~%s\033[0m$ ", user->pw_name, hostname, pwd + homelenth);
            } else {
                printf("\033[32m%s@%s\033[0m" ":" "\033[31m%s\033[0m$ ", user->pw_name, hostname, pwd);    
            }
        }
        fgets(cmd, 1000, stdin);
        cmd[strlen(cmd) - 1] = 0;
        parsecmd(cmd);
    }
    return 0;
}
