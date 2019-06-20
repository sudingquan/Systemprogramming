#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#define MAX_SIZE 1024

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
		//perror("connect() error");
		//DBG("connect() error : %s!\n", stderror(errno));
		return -1;
	}
	return sockfd;

}

int main() {
	int socket_fd;
	int port = 8888;
	char ip_addr[20] = "39.105.82.248"; 
	//char ip_addr[20] = "192.168.2.90";
    char file_name[100];
    char data[MAX_SIZE + 5];
    printf("Enter the file you want to send : ");
    scanf("%s", file_name);
    FILE *fp = NULL;
    fp = fopen(file_name, "rb");
    if(fp == NULL){
        printf("open file error\n");
        exit(1);
    }
	socket_fd = socket_connect(port, ip_addr);
    if(socket_fd > 0){
        printf("connect success!\n");
    }
    printf("send filename...\n");
    send(socket_fd, file_name, 100, 0);
    printf("send file...\n");
    while (1) {
        int i;
        i = fread(data, 1, MAX_SIZE, fp);
        send(socket_fd, data, i, 0);
        memset(data, 0, sizeof(data));
        if (feof(fp)) {
            fclose(fp);
            fp = NULL;
            break;
        }
    }
    printf("send file success\n");
	close(socket_fd);
	return 0;

}
