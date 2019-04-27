/*************************************************************************
	> File Name: terminal_2.0.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 六  4/27 18:09:16 2019
 ************************************************************************/

#include <stdio.h>
#include <passwd.h>

void getUsername() {
    struct passwd *pwd = getpwuid(getuid());

}
