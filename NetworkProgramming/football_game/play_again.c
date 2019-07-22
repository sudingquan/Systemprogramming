/*************************************************************************
	> File Name: play_again.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 二  7/16 14:08:21 2019
 ************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#define ASK "Do you want another try"
#define TRY 3
#define SLEEPTIME 5

bool get_response(char *);
void set_crmode();
void tty_mode(int);
void set_nonblock();

int main() {
    bool response;
    tty_mode(0);
    set_crmode();
    set_nonblock();
    response = get_response(ASK);
    tty_mode(1);
    printf("\nresponse = %d\n", response);
    return 0;
}

bool get_response(char *ask) {
    printf("%s (y/n)?", ask);
    fflush(stdout);
    while (1) {
        char ch;
        char get_char();
        fd_set set;
        struct timeval tv;
        int ret;
        FD_ZERO(&set);
        FD_SET(0, &set);

        tv.tv_sec = SLEEPTIME;
        tv.tv_usec = 0;
        ret = select(1, &set, NULL, NULL, &tv);
        if (ret == -1) {
            perror("select()");
            return false;
        } else if (ret) {
            ch = get_char();
            if (ch == 'Y' || ch == 'y') {
                return true;
            } else if (ch == 'N' || ch == 'n') {
                return false;
            } else {
                continue;
            }
        } else {
            printf("\nTimeout");
            return false;
        }
    }
}

char get_char() {
    char ch;
    while ((ch = getchar()) != EOF && strchr("YyNn", ch) == NULL);
    return ch;
}

void set_crmode() {
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ECHOE;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &ttystate);
}

void tty_mode(int how) {
    static struct termios orignal_mode;
    if (!how) {
        tcgetattr(0, &orignal_mode);
    } else {
        tcsetattr(0, TCSANOW, &orignal_mode);
    }
}

void set_nonblock() {
    int flag = fcntl(0, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(0, F_SETFL, flag);
}
