/*************************************************************************
	> File Name: tcp_server.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 二  6/ 4 22:21:40 2019
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
#include <pthread.h>
#define PORT 8888
#define MAX_SIZE 1024

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

void *recv_file(void *client_socket) {
    char file_name[100];
    char data[MAX_SIZE + 5];
    int ret = recv(*(int *)client_socket, file_name, 100, 0);
    if (ret != 100) {
        printf("recv file_name failed\n");
        close(*(int *)client_socket);
        return NULL;
    }
    FILE *fp = NULL;
    fp = fopen(file_name,"wb");
    if (fp == NULL) {
        printf("open file error\n");
        close(*(int *)client_socket);
        fclose(fp);
        fp = NULL;
        exit(1);
    }
    while (1) {
        int i = recv(*(int *)client_socket, data, MAX_SIZE, 0);
        if (i == 0) {
            fclose(fp);
            fp = NULL;
            break;
        }
        fwrite(data, 1, i, fp);
        memset(data, 0, sizeof(data));
    }
    close(*(int *)client_socket);
    return NULL;
}

int main() {
    int listen_socket = creat_listen_socket();
    if (listen_socket < 0) {
        exit(1);
    }
    while (1) {    
        int client_socket = wait_client(listen_socket);
        if (client_socket < 0) {
            exit(1);
        }
        pthread_t pt;
        pthread_create(&pt, NULL, recv_file, (void *)&client_socket);
    }
    close(listen_socket);
    return 0;
}
