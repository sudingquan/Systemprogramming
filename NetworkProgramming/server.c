/*************************************************************************
	> File Name: server.c
	> Author : sudingquan
	> Mail : 1151015256@qq.com
	> Created Time: Sat Jun 22 14:25:05 2019
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

#define PORT 8888
#define LEFTEDGE 2
#define RIGHTEDGE 50
#define ROW 30
#define TIME 10000

typedef struct map {
    int left;
    int right;
    int row;
} map;

typedef struct position {
    int x;
    int y;
} position;

map map1;
position pos;
char ball[2] = "o";
char blank[2] = " ";

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
    int client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &addrlen);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    }
    printf("connection to the client %s successful\n", inet_ntoa(client_addr.sin_addr));
    return client_socket;
}

void init_map() {
    for (int i = LEFTEDGE; i <= RIGHTEDGE; i++) {
        move(0, i);
        addstr("-");
    }
    for (int i = LEFTEDGE; i <= RIGHTEDGE; i++) {
        move(ROW + 1, i);
        addstr("-");
    }
    for (int i = 0; i <= ROW + 1; i++) {
        move(i, LEFTEDGE - 1);
        addstr("|");
    }
    for (int i = 0; i <= ROW + 1; i++) {
        move(i, RIGHTEDGE + 1);
        addstr("|");
    }
    return;
}

void *send_ball_position(void *client_socket) {
    while (1) {
        if (send(*(int *)client_socket, &pos, sizeof(pos), 0) < 0) {
            printf("send ball position failed\n");
            close(*(int *)client_socket);
            //exit(1);
            return NULL;
        }
        usleep(TIME);
    }
    return NULL;
}

void *recv_dir(void *client_socket) {
    while (1) {
        char ch;
        if (recv(*(int *)client_socket, &ch, sizeof(ch), 0) < 0) {
            printf("recv dir failed\n");
            close(*(int *)client_socket);
            //exit(1);
            return NULL;
        }
        switch(ch) {
            case 'w': pos.x -= (pos.x > 1);
                break;
            case 'a': pos.y -= (pos.y > LEFTEDGE);
                break;
            case 's': pos.x += (pos.x < ROW);
                break;
            case 'd': pos.y += (pos.y < RIGHTEDGE);
                break;
            default: break;
        }
    }
    return NULL;
}

void send_map(int client_socket) {
    if (send(client_socket, &map1, sizeof(map1), 0) < 0) {
        printf("send map failed\n");
        close(client_socket);
        //exit(1);
    }
    return;
}

//void *update(void *argv) {
//    while (1) {
//        move(pos.x, pos.y);
//        int x = pos.x;
//        int y = pos.y;
//        addstr(ball);
//        move(LINES - 1, COLS - 1);
//        refresh();
//        move(x, y);
//        addstr(blank);
//    }
//    return NULL;
//}
//
int main() {
    pthread_t send[100];
    pthread_t recv[100];
    pos.x = 1;
    pos.y = LEFTEDGE;
    initscr();
    clear();
    map1.left = LEFTEDGE;
    map1.right = RIGHTEDGE;
    map1.row = ROW;
    init_map();
    int listen_socket = creat_listen_socket();
    if (listen_socket < 0) {
        exit(1);
    }
    int client_socket[100];
    int i = 0;
    while (1) {
        client_socket[i] = wait_client(listen_socket);
        if (client_socket[i] < 0) {
            printf("connection failed\n");
            continue;
        }
        send_map(client_socket[i]);
        pthread_create(&send[i], NULL, send_ball_position, (void *)&client_socket[i]);
        pthread_create(&recv[i], NULL, recv_dir, (void *)&client_socket[i]);
        i++;
    }
    return 0;
}
