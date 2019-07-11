/*************************************************************************
	> File Name: 1.epoll.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 四  7/11 20:46:44 2019
 ************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#define PORT 8888
#define MAX_EVENTS 10
#define MAX_BUFF 100
#define MAX_CLIENTS 1000

typedef struct UserData {
    int fd;
    char name[20];
    char msg[MAX_BUFF];
} UserData;

UserData userdata[MAX_CLIENTS];

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
    }
    return listen_socket;
}

int main() {
    struct epoll_event ev, events[MAX_EVENTS];
    int listen_sock, conn_sock, nfds, epollfd;
    struct sockaddr_in client_addr;
    unsigned int addrlen = sizeof(client_addr);
    char buff[MAX_BUFF] = {0};
    int num;

    /* Code to set up listening socket, 'listen_sock',
       (socket(), bind(), listen()) omitted */
    listen_sock = creat_listen_socket();
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    userdata[listen_sock].fd = listen_sock;
    ev.data.ptr = &userdata[listen_sock];

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int n = 0; n < nfds; ++n) {
            UserData *user_evs = (UserData *)events[n].data.ptr;
            UserData *user_ev = (UserData *)ev.data.ptr;
            if (user_evs->fd == listen_sock) {
                conn_sock = accept(listen_sock, (struct sockaddr *) &client_addr, &addrlen);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                int imode = 1;
                ioctl(conn_sock, FIONBIO, &imode); //将新连接设置成非阻塞状态
                ev.events = EPOLLIN | EPOLLET; //将新连接加入epoll监听
                userdata[conn_sock].fd = conn_sock;
                ev.data.ptr = &userdata[conn_sock];
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else if (events[n].events & EPOLLIN) {
				int client_sock = user_evs->fd;
                int num;
                memset(&userdata[client_sock], 0, sizeof(UserData));
                num = recv(client_sock, user_evs->msg, MAX_BUFF, 0);
                ev.events = EPOLLOUT;
                userdata[client_sock].fd = client_sock;
                ev.data.ptr = &userdata[client_sock];
            	if (num <= 0) {
                	epoll_ctl(epollfd, EPOLL_CTL_DEL, client_sock, &ev);
                    getpeername(client_sock, (struct sockaddr *)&client_addr, &addrlen);
                    printf("<%s> exit !\n", inet_ntoa(client_addr.sin_addr));
                    memset(&userdata[client_sock], 0, sizeof(UserData));
                	close(client_sock);
                	continue;
				}
                epoll_ctl(epollfd, EPOLL_CTL_MOD, client_sock, &ev);
            } else if (events[n].events & EPOLLOUT) {
				int client_sock = user_evs->fd;
                ev.events = EPOLLIN;
                userdata[client_sock].fd = client_sock;
                ev.data.ptr = &userdata[client_sock];
                getpeername(client_sock, (struct sockaddr *)&client_addr, &addrlen);
                printf("<%s> : %s", inet_ntoa(client_addr.sin_addr), user_evs->msg);
                num = send(client_sock, user_evs->msg, strlen(user_evs->msg), 0);
                if (num > 0 ) {
                    printf(" [send !]\n");
                } else {
                    printf(" [send x]\n");
                }
                epoll_ctl(epollfd, EPOLL_CTL_MOD, client_sock, &ev);
            }
        }
    }
    return 0;
}
