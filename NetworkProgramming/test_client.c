/*************************************************************************
	> File Name: client.c
    > Author: sudingquan
	> Mail: 1151015256@qq.com
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
#define MAX_N 200

#define TIME 50000

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

int socket_connect(int port, char *host) {
	int sockfd;
	struct sockaddr_in dest_addr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() error");
		return -1;
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(host);

	//DBG("Connetion TO %s:%d\n",host,port);
	//fflush(stdout);
	if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
		perror("connect() error");
		//DBG("connect() error : %s!\n", stderror(errno));
		return -1;
	}
	return sockfd;

}

void init_map(int leftedge, int rightedge, int row) {
    for (int i = leftedge; i <= rightedge + 1; i++) {
        move(0, i);
        addstr("-");
    }
    for (int i = leftedge; i <= rightedge + 1; i++) {
        move(row + 1, i);
        addstr("-");
    }
    for (int i = 0; i <= row + 1; i++) {
        move(i, leftedge - 1);
        addstr("|");
    }
    for (int i = 0; i <= row + 1; i++) {
        move(i, rightedge + 1);
        addstr("|");
    }
    return;
}

void *recv_position(void *socket_fd) {
    while (1) {
        int ret;
        if ((ret = recv(*(int *)socket_fd, &pos, sizeof(pos), 0)) < 0) {
            printf("recv ball position failed\n");
            close(*(int *)socket_fd);
            exit(1);
        }
        printf("ret = %d sizeof(pos) = %d\n", ret, sizeof(pos));
        printf("num = %d, pos.px = %d, pos.py = %d\n", pos.flag, pos.px[pos.uid[pos.flag - 1]], pos.py[pos.uid[pos.flag - 1]]);
        usleep(TIME);
    }
    return NULL;
}

void *send_dir(void *socket_fd) {
    char ch;
    while (1) {
        ch = getchar();
        if (send(*(int *)socket_fd, &ch, sizeof(ch), 0) < 0) {
            printf("send dir failed\n");
            close(*(int *)socket_fd);
            exit(1);
        }
    }
    return NULL;
}

void recv_map(int socket_fd) {
    if (recv(socket_fd, &map1, sizeof(map1), 0) < 0) {
        printf("recv map failed\n");
        close(socket_fd);
        exit(1);
    }
}

void *draw(void *arg) {
    while (1) {
        int temp1[MAX_N];
        int temp2[MAX_N];
        for (int i = 0; i < pos.flag; i++) {
            move(pos.px[pos.uid[i]], pos.py[pos.uid[i]]);
            temp1[pos.uid[i]] = pos.px[pos.uid[i]];
            temp2[pos.uid[i]] = pos.py[pos.uid[i]];
            addstr(person);
        }
        move(pos.bx,pos.by);
        int x,y;
        x = pos.bx;
        y = pos.by;
        addstr(ball);
        move(LINES - 1, COLS - 1);
        refresh();
        move(x, y);
        addstr(blank);
        for (int i = 0; i < pos.flag; i++) {
            move(temp1[pos.uid[i]], temp2[pos.uid[i]]);
            addstr(blank);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage:./client ip port\n");
        exit(1);
    }
	int socket_fd;
	int port = atoi(argv[2]);
    char ip_addr[20];
    if (strcmp(argv[1], "aliyun") == 0) {
        strcpy(ip_addr, "39.105.82.248");
    } else {
        strcpy(ip_addr, argv[1]);
    }
	//char ip_addr[20] = "192.168.2.90";
	socket_fd = socket_connect(port, ip_addr);
    if(socket_fd > 0){
        printf("connect success!\n");
    } else {
        close(socket_fd);
        exit(1);
    }
    //recv_map(socket_fd);
	//close(socket_fd);
    //initscr();
    //clear();
    //init_map(map1.left, map1.right, map1.row);
    refresh();
    pthread_t recv, send, draw_p_b;
    pthread_create(&recv, NULL, recv_position, (void *)&socket_fd);
    pthread_create(&send, NULL, send_dir, (void *)&socket_fd);
    //pthread_create(&draw_p_b, NULL, draw, NULL);
    pthread_join(recv, NULL);
	return 0;
}
