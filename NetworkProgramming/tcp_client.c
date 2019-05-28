#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
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
	int  socket_fd;
	struct passwd *pwd;
    pwd = getpwuid(getuid());	
	char ip_addr[20] = "192.168.1.40";
	int port = 8888;
    char username[20] = {0};
    strcpy(username, pwd->pw_name);
	socket_fd = socket_connect(port, ip_addr);
        if(socket_fd > 0){
            printf("success!\n");
        }
    send(socket_fd, username, strlen(username), 0);
	close(socket_fd);
	return 0;

}
