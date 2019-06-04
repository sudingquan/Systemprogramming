/*************************************************************************
	> File Name: tcp_server.c
	> Author: sudingquan
	> Mail: 1151015256@qq.com
	> Created Time: 二  6/ 4 22:21:40 2019
 ************************************************************************/

#include <stdio.h>
#include <sys/socketmZ>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#define PORT 8888

int socket_connect() {
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
    if (connect(sockfd, (struct sockaddr *)&dest_addr), sizeof(dest_addr) < 0) {
        return -1;
    }
    return sockfd;
}

int main() {
    int sockfd;
    struct sockaddr_in from_addr, my_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
        return -1;
    }
    memset(&from_addr, 0, sizeof(from_addr));
    my_addr.sin_family = AF_INET;

    return 0;
}
