/*************************************************************************
	> File Name: tcp_select_acpt.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 日  7/ 7 16:12:40 2019
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
#include <sys/ioctl.h>
#define PORT 8888
#define MAX_CLIENTS 40
#define MAX_BUFF 1024

typedef struct server_data {
    int client_cnt;
    int client_fds[MAX_CLIENTS];
    fd_set all_fds;
    int maxfd;
} server_data;

server_data *s_d = NULL;

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

int wait_client(int listen_socket) {
    struct sockaddr_in client_addr;
    printf("wait for client...\n");
    unsigned int addrlen = sizeof(client_addr);

    unsigned long imode = 1;

    //ioctl(listen_socket, FIONBIO, &imode);

    int client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &addrlen);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    }
    printf("connection to the client %s successful\n", inet_ntoa(client_addr.sin_addr));
    return client_socket;
}

void accept_client(int listen_socket) {
    int client_fd = -1;
    client_fd = wait_client(listen_socket);
    if (client_fd == -1) {
        perror("socket_accept");
        return ;
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (s_d->client_fds[i] == -1) {
            s_d->client_fds[i] = client_fd;
            s_d->client_cnt++;
            break;
        }
    }
    return ;
}

void recv_send(fd_set *readfds) {
    int client_fd;
    char buff[MAX_BUFF] = {0};
    int num;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    for (int i = 0; i < s_d->client_cnt; i++) {
        client_fd = s_d->client_fds[i];
        if (client_fd < 0) {
            continue;
        }
        if (FD_ISSET(client_fd, readfds)) {
            num = recv(client_fd, buff, MAX_BUFF, 0);
            if (num <= 0) {
                FD_CLR(client_fd, &(s_d->all_fds));
                close(client_fd);
                s_d->client_fds[i] = -1;
                s_d->client_cnt--;
                continue;
            } else {
                getpeername(client_fd, (struct sockaddr *)&client, &len);
                printf("<%s> : %s", inet_ntoa(client.sin_addr), buff);
                num = send(client_fd, buff, strlen(buff), 0);
                if (num > 0 ) {
                    printf(" [send !]\n");
                } else {
                    printf(" [send x]\n");
                }
            }
        }
    }
}

void do_client(int listen_socket) {
    int client_fd = -1, ret;
    fd_set *readfds = &(s_d->all_fds);
    struct timeval tm;
    while (1) {
        FD_ZERO(readfds);
        FD_SET(listen_socket, readfds);
        s_d->maxfd = listen_socket;
        tm.tv_sec = 5;
        tm.tv_usec = 0;
        for(int i = 0; i < s_d->client_cnt; i++) {
            client_fd = s_d->client_fds[i];
            if (client_fd != -1) {
                FD_SET(client_fd, readfds);
            }
            s_d->maxfd = (client_fd > s_d->maxfd ? client_fd : s_d->maxfd);
        }
        ret = select(s_d->maxfd + 1, readfds, NULL, NULL, &tm);
        if (ret < 0) {
            perror("select");
            return;
        } else if (ret == 0) {
            printf("timeout!\n");
            continue;
        } else {
            if (FD_ISSET(listen_socket, readfds)) {
                printf("Before Accept!\n");
                accept_client(listen_socket);
                printf("After Accept!\n");
            } else {
                recv_send(readfds);
            }
        }
    }
}

int main() {
    int listen_socket = creat_listen_socket();
    if (listen_socket < 0) {
        exit(1);
    }
    s_d = (server_data *)malloc(sizeof(server_data));

    for (int i = 0; i < MAX_CLIENTS; i++) {
        s_d->client_fds[i] = -1;
    }
    do_client(listen_socket);
    close(listen_socket);
    return 0;
}
