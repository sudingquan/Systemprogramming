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
#define PORT 8888

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

void handler(int sig)
{
	
	while (waitpid(-1,  NULL,   WNOHANG) > 0)
	{
		printf ("成功处理一个子进程的退出\n");
	}
}

int main() {
    int listen_socket = creat_listen_socket();
    if (listen_socket < 0) {
        exit(1);
    }
    while(1) {    
        int client_socket = wait_client(listen_socket);
        if (client_socket < 0) {
            exit(1);
        }
        signal(SIGCHLD,  handler);    //处理子进程，防止僵尸进程的产生
        int pid = fork();
		if(pid == -1)
		{
			perror("fork");
			break;
		}
		if(pid > 0)
		{
			close(client_socket);
			continue;
		}
		if(pid == 0)
		{
			close(listen_socket);
            char username[20];
            recv(client_socket, username, 20, 0);
            printf("%s\n",username);
            close(client_socket);
			break;
		}
    }
    close(listen_socket);
    return 0;
}
