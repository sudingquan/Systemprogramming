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

	//printf("Connetion TO %s:%d\n",host,port);
	//fflush(stdout);
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(sockfd, &wfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 300000;
    unsigned long imode = 1;

    int error = -1;
    int len = sizeof(error);
    int ret = -1;

    ioctl(sockfd, FIONBIO, &imode);
    int n = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (n < 0 && errno == EINPROGRESS) {
        retval = select(sockfd + 1, NULL, &wfds, NULL, &timeout);
        if (retval > 0) {
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0) {
                perror("getsockopt");
                ret = -1;
            }
            if (error == 0) {
                ret = 0;
            } else {
                ret = -1;
            }
        }
    }
    close(sockfd);
    return ret;
}

int main() {
	int  socket_fd;
	char ip_addr[20] = {0}; 
	int port = 8888;
    for (int i = 30; i <= 60; i++) {
        sprintf(ip_addr, "192.168.1.%d", i);
        if (socket_connect(port, ip_addr) == 0) {
            printf("%s\n", ip_addr);
        }
    }
	return 0;

}
