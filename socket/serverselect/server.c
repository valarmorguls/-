/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Fri 17 Feb 2023 02:54:11 PM CST
 ************************************************************************/

#include<stdio.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define MAXLINE 80
#define SERV_PORT 8000

int main() {
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
 
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
 
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
 
    if (listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    fd_set rfds, rset, wfds, wset;
	FD_ZERO(&rfds);
	FD_SET(listenfd, &rfds);
	FD_ZERO(&wfds);

	int max_fd = listenfd;

	while (1) {
		rset = rfds;
		wset = wfds;
		int nready = select(max_fd+1, &rset, &wset, NULL, NULL);
		if (FD_ISSET(listenfd, &rset)) { //
			struct sockaddr_in client;
		    socklen_t len = sizeof(client);
		    if ((connfd = accept(listenfd, (struct sockaddr *)&client, &len)) == -1) {
		        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
		        return 0;
		    }

			FD_SET(connfd, &rfds);

			if (connfd > max_fd) max_fd = connfd;

			if (--nready == 0) continue;

		}

		int i = 0;
		for (i = listenfd+1;i <= max_fd;i ++) {
			if (FD_ISSET(i, &rset)) { //
				n = recv(i, buff, MAXLINE, 0);
		        if (n > 0) {
		            buff[n] = '\0';
		            printf("recv msg from client: %s\n", buff);
					FD_SET(i, &wfds);
					//reactor
					//send(i, buff, n, 0);
		        } else if (n == 0) { //
					FD_CLR(i, &rfds);
					//printf("disconnect\n");
		            close(i);
		        }
				if (--nready == 0) break;
			} else if (FD_ISSET(i, &wset)) {
				send(i, buff, n, 0);
				FD_SET(i, &rfds);
			}
		}
	}
    return 0;
}
