/*************************************************************************
	> File Name: game_epoll_server.cpp
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 五  7/12 00:35:29 2019
 ************************************************************************/

#include <stdio.h>
#include <curses.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <signal.h>

#define PORT 8888
#define LEFTEDGE 1
#define RIGHTEDGE 151
#define ROW 35
#define TIME 10000
#define MAX_N 1000
#define MAX_STEP 15

typedef struct Map {
    int left;
    int right;
    int row;
} Map;

typedef struct Position {
    int px[MAX_N];
    int py[MAX_N];
    int bx;
    int by;
    int num;
    int uid[MAX_N];
} Position;

struct Move {
    int x;
    int y;
} mv;

struct itimerval timer;

int epollfd;

int max_num;

Map map1;
Position pos;

int creat_listen_socket() {
    int listen_socket;
    if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
        return -1;
    }
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_socket, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("bind");
        return -1;
    }
    if (listen(listen_socket, 5) < 0) {
        perror("listen");
        return -1;
    }
    return listen_socket;
}

void ready_to_send() {
    for (int i = 0; i < max_num; i++) {
        if (pos.uid[i] == -1) continue;
        struct epoll_event ev;
        ev.events = EPOLLOUT;
        ev.data.fd = pos.uid[i];
        epoll_ctl(epollfd, EPOLL_CTL_MOD, pos.uid[i], &ev);
    }
    return ;
} 

int send_position(int client_socket) {
    int ret;
    if ((ret = send(client_socket, &pos, sizeof(pos), 0)) < 0) {
        return -1;
    }
    //printf("ret = %d\n", ret);
    return 0;
}

void ball_move() {
    if (pos.bx > 1 && pos.bx < ROW && pos.by < RIGHTEDGE && pos.by > LEFTEDGE) {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec += 20000;
        if (timer.it_value.tv_usec > MAX_STEP * 20000) {
            return ;
        }
        if(setitimer(ITIMER_REAL, &timer, NULL) < 0) {
            printf("Set timer failed!\n");
        }
        pos.bx += mv.x;
        pos.by += mv.y;
        ready_to_send(epollfd);
    } else {
        pos.bx = (1 + ROW) / 2;
        pos.by = (RIGHTEDGE - LEFTEDGE) / 2;
        mv.x = 0;
        mv.y = 0;
        ready_to_send(epollfd);
    }
    return ;
}

void shoot_ball() {
    if (mv.x != 0 || mv.y != 0) {
        signal(SIGALRM, ball_move);
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 20000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        if(setitimer(ITIMER_REAL, &timer, NULL) < 0) {
            printf("Set timer failed!\n");
        }
    }
    return ;
}

int recv_dir(int client_socket) {
    char ch;
    if (recv(client_socket, &ch, sizeof(ch), 0) <= 0) {
       return -1;
    }
    switch(ch) {
        case 'w': {
            if (pos.px[client_socket] - 1 == pos.bx && pos.py[client_socket] == pos.by)  {
                pos.bx -= (pos.bx > 1);
                pos.px[client_socket] -= (pos.px[client_socket] > 1 && pos.bx > 1);
            } else {
                pos.px[client_socket] -= (pos.px[client_socket] > 1);
            }
            break;
        } 
        case 'a': {
            if (pos.py[client_socket] - 1 == pos.by && pos.px[client_socket] == pos.bx) {
                pos.by -= (pos.by > LEFTEDGE);
                pos.py[client_socket] -= (pos.py[client_socket] > LEFTEDGE && pos.by > LEFTEDGE);
            } else {
                pos.py[client_socket] -= (pos.py[client_socket] > LEFTEDGE);
            }
            break;
        } 
        case 's': {
            if (pos.px[client_socket] + 1 == pos.bx && pos.py[client_socket] == pos.by) {
                pos.bx += (pos.bx < ROW);
                pos.px[client_socket] += (pos.px[client_socket] < ROW && pos.bx < ROW);
            } else {
                pos.px[client_socket] += (pos.px[client_socket] < ROW);
            }
            break;
        } 
        case 'd': {
            if (pos.py[client_socket] + 1 == pos.by && pos.px[client_socket] == pos.bx) {
                pos.by += (pos.by < RIGHTEDGE);
                pos.py[client_socket] += (pos.py[client_socket] < RIGHTEDGE && pos.by < RIGHTEDGE);
            } else {
                pos.py[client_socket] += (pos.py[client_socket] < RIGHTEDGE);
            }
            break;
        } 
        case ' ': {
            if (pos.px[client_socket] - 1 == pos.bx && pos.py[client_socket] - 1 == pos.by) {
                mv.x = -1;
                mv.y = -1;
                shoot_ball();
            } else if (pos.px[client_socket] - 1 == pos.bx && pos.py[client_socket] + 0 == pos.by) {
                mv.x = -1;
                mv.y = 0;
                shoot_ball();
            } else if (pos.px[client_socket] - 1 == pos.bx && pos.py[client_socket] + 1 == pos.by) {
                mv.x = -1;
                mv.y = 1;
                shoot_ball();
            } else if (pos.px[client_socket] + 0 == pos.bx && pos.py[client_socket] - 1 == pos.by) {
                mv.x = 0;
                mv.y = -1;
                shoot_ball();
            } else if (pos.px[client_socket] + 0 == pos.bx && pos.py[client_socket] + 1 == pos.by) {
                mv.x = 0;
                mv.y = 1;
                shoot_ball();
            } else if (pos.px[client_socket] + 1 == pos.bx && pos.py[client_socket] - 1 == pos.by) {
                mv.x = 1;
                mv.y = -1;
                shoot_ball();
            } else if (pos.px[client_socket] + 1 == pos.bx && pos.py[client_socket] + 0 == pos.by) {
                mv.x = 1;
                mv.y = 0;
                shoot_ball();
            } else if (pos.px[client_socket] + 1 == pos.bx && pos.py[client_socket] + 1 == pos.by) {
                mv.x = 1;
                mv.y = 1;
                shoot_ball();
            }
            break;
        }
        default: break;
    }
    return 0;
}

void send_map(int client_socket) {
    if (send(client_socket, &map1, sizeof(map1), 0) < 0) {
        printf("send map failed\n");
        close(client_socket);
    }
    return;
}

void *temp1(void *i) {
    while (1){
        sleep(20000);
    }
}

int main() {
    struct epoll_event ev, events[MAX_N];
    int listen_sock, conn_sock, nfds;
    struct sockaddr_in client_addr;
    unsigned int addrlen = sizeof(client_addr);

    listen_sock = creat_listen_socket();
    if (listen_sock < 0) {
        printf("create listen sock failed !\n");
        exit(EXIT_FAILURE);
    }

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    pos.bx = (1 + ROW) / 2;
    pos.by = (RIGHTEDGE - LEFTEDGE) / 2;
    pos.num = 0;

    map1.left = LEFTEDGE;
    map1.right = RIGHTEDGE;
    map1.row = ROW;

    pthread_t temp;
    pthread_create(&temp, NULL, temp1, NULL);

    while (1) {
        sigset_t sigmask;
        if(sigemptyset(&sigmask) ==  -1) {
            perror("sigemptyset");
            exit(1);
        }
        if(sigaddset(&sigmask,SIGALRM) == -1) {
            perror("sigaddset");
            exit(1);
        }
        nfds = epoll_pwait(epollfd, events, MAX_N, -1, &sigmask);
        if (nfds == -1) {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }
        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == listen_sock) {
                conn_sock = accept(listen_sock, (struct sockaddr *) &client_addr, &addrlen);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                int imode = 1;
                ioctl(conn_sock, FIONBIO, &imode); //将新连接设置成非阻塞状态
                ev.events = EPOLLIN | EPOLLOUT; //将新连接加入epoll监听
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
                pos.uid[pos.num] = conn_sock;
                pos.num++;
                max_num = pos.num;
                srand(time(NULL));
                pos.px[conn_sock] = rand() % (ROW - 1) + 1;
                pos.py[conn_sock] = rand() % (RIGHTEDGE - LEFTEDGE) + LEFTEDGE;
                send_map(conn_sock);
            } else if (events[n].events & EPOLLIN) {
			    int client_sock = events[n].data.fd;
                int ret;
                printf("<%s> : recv_dir \n", inet_ntoa(client_addr.sin_addr));
                ret = recv_dir(client_sock);
                ready_to_send();
                ev.events = EPOLLOUT;
                ev.data.fd = client_sock;
                getpeername(client_sock, (struct sockaddr *)&client_addr, &addrlen);
                if (ret < 0) {
                	epoll_ctl(epollfd, EPOLL_CTL_DEL, client_sock, &ev);
                    printf("<%s> exit !\n", inet_ntoa(client_addr.sin_addr));
                    pos.num -= 1;
                    //pos.px[client_sock] = -1;
                    //pos.py[client_sock] = -1;
                    for (int i = 0; i < max_num; i++) {
                        if(pos.uid[i] != client_sock) continue;
                        pos.uid[i] = -1;
                    }
                	close(client_sock);
                	continue;
			    }
                printf("<%s> : recv success !\n", inet_ntoa(client_addr.sin_addr));
                epoll_ctl(epollfd, EPOLL_CTL_MOD, client_sock, &ev);
            } else if (events[n].events & EPOLLOUT) {
				int client_sock = events[n].data.fd;
                int ret;
                ret = send_position(client_sock);
                ev.events = EPOLLIN;
                ev.data.fd = client_sock;
                if (ret < 0 ) {
                    printf("send ball position failed\n");
                    continue;
                }
                printf("send success !\n");
                epoll_ctl(epollfd, EPOLL_CTL_MOD, client_sock, &ev);
            }
        }
    }
    close(epollfd);
    return 0;
}
