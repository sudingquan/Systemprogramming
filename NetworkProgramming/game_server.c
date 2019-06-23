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
#include <time.h>

#define PORT 8888
#define LEFTEDGE 1
#define RIGHTEDGE 56
#define ROW 35
#define TIME 10000
#define MAX_N 100
#define MAX_STEP 15

typedef struct map {
    int left;
    int right;
    int row;
} map;

typedef struct position {
    int px[MAX_N];
    int py[MAX_N];
    int bx;
    int by;
    int flag;
    int uid[MAX_N];
} position;

map map1;
position pos;

char ball[2] = "o";
char blank[2] = " ";
char person[10] = "R";

int client_socket[MAX_N];

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

void *send_position(void *client_socket) {
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

void shoot_ball(int x, int y) {
    for (int i = 0; i < MAX_STEP; i++) {
        if (pos.bx > 1 && pos.bx < ROW && pos.by < RIGHTEDGE && pos.by > LEFTEDGE) {
            pos.bx += x;
            pos.by += y;
        } else {
            pos.bx = (1 + ROW) / 2;
            pos.by = (RIGHTEDGE - LEFTEDGE) / 2;
            break;
        }
        usleep(10000);
    }
    return;
}

void *recv_dir(void *i) {
    while (1) {
        char ch;
        if (recv(client_socket[*(int *)i], &ch, sizeof(ch), 0) < 0) {
            printf("recv dir failed\n");
            close(client_socket[*(int *)i]);
            //exit(1);
            return NULL;
        }
        switch(ch) {
            case 'w': {
                if (pos.px[*(int *)i] - 1 == pos.bx && pos.py[*(int *)i] == pos.by)  {
                    pos.bx -= (pos.bx > 1);
                    pos.px[*(int *)i] -= (pos.px[*(int *)i] > 1 && pos.bx > 1);
                } else {
                    pos.px[*(int *)i] -= (pos.px[*(int *)i] > 1);
                }
                break;
            } 
            case 'a': {
                if (pos.py[*(int *)i] - 1 == pos.by && pos.px[*(int *)i] == pos.bx) {
                    pos.by -= (pos.by > LEFTEDGE);
                    pos.py[*(int *)i] -= (pos.py[*(int *)i] > LEFTEDGE && pos.by > LEFTEDGE);
                } else {
                    pos.py[*(int *)i] -= (pos.py[*(int *)i] > LEFTEDGE);
                }
                break;
            } 
            case 's': {
                if (pos.px[*(int *)i] + 1 == pos.bx && pos.py[*(int *)i] == pos.by) {
                    pos.bx += (pos.bx < ROW);
                    pos.px[*(int *)i] += (pos.px[*(int *)i] < ROW && pos.bx < ROW);
                } else {
                    pos.px[*(int *)i] += (pos.px[*(int *)i] < ROW);
                }
                break;
            } 
            case 'd': {
                if (pos.py[*(int *)i] + 1 == pos.by && pos.px[*(int *)i] == pos.bx) {
                    pos.by += (pos.by < RIGHTEDGE);
                    pos.py[*(int *)i] += (pos.py[*(int *)i] < RIGHTEDGE && pos.by < RIGHTEDGE);
                } else {
                    pos.py[*(int *)i] += (pos.py[*(int *)i] < RIGHTEDGE);
                }
                break;
            } 
            case ' ': {
                if (pos.px[*(int *)i] - 1 == pos.bx && pos.py[*(int *)i] - 1 == pos.by) {
                    shoot_ball(-1, -1);
                } else if (pos.px[*(int *)i] - 1 == pos.bx && pos.py[*(int *)i] + 0 == pos.by) {
                    shoot_ball(-1, 0);
                } else if (pos.px[*(int *)i] - 1 == pos.bx && pos.py[*(int *)i] + 1 == pos.by) {
                    shoot_ball(-1, 1);
                } else if (pos.px[*(int *)i] + 0 == pos.bx && pos.py[*(int *)i] - 1 == pos.by) {
                    shoot_ball(0, -1);
                } else if (pos.px[*(int *)i] + 0 == pos.bx && pos.py[*(int *)i] + 1 == pos.by) {
                    shoot_ball(0, 1);
                } else if (pos.px[*(int *)i] + 1 == pos.bx && pos.py[*(int *)i] - 1 == pos.by) {
                    shoot_ball(1, -1);
                } else if (pos.px[*(int *)i] + 1 == pos.bx && pos.py[*(int *)i] + 0 == pos.by) {
                    shoot_ball(1, 0);
                } else if (pos.px[*(int *)i] + 1 == pos.bx && pos.py[*(int *)i] + 1 == pos.by) {
                    shoot_ball(1, 1);
                }
                break;
            }
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
    pthread_t recv_d[MAX_N];
    pthread_t send[MAX_N];
    pos.bx = (1 + ROW) / 2;
    pos.by = (RIGHTEDGE - LEFTEDGE) / 2;
    pos.flag = 0;
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
    int i = 0;
    while (1) {
        client_socket[i] = wait_client(listen_socket);
        if (client_socket[i] < 0) {
            printf("connection failed\n");
            continue;
        }
        pos.flag += 1;
        pos.uid[i] = i;
        srand(time(NULL));
        pos.px[i] = rand() % (ROW - 1) + 1;
        pos.py[i] = rand() % (RIGHTEDGE - LEFTEDGE) + LEFTEDGE;
        send_map(client_socket[i]);
        pthread_create(&send[i], NULL, send_position, (void *)&client_socket[i]);
        pthread_create(&recv_d[i], NULL, recv_dir, (void *)&pos.uid[i]);
        i++;
    }
    return 0;
}
