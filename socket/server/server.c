/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Fri 17 Feb 2023 01:37:04 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#define MAXLINE 80
#define SERV_PORT 8000


int main() {
    struct sockaddr_in serveraddr, cliaddr;
    int listenfd, connfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int n, i;
    socklen_t cliaddr_len; 
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        exit(1);
    }
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind");
        exit(1);
    }
    if (listen(listenfd, 3) < 0) {
        perror("listen");
        exit(1);
    }
    printf("================Waiting for client's request=================\n");
    while (1) {
        cliaddr_len = sizeof(cliaddr); 
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        if (connfd < 0) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            exit(1);
        }
        printf("received from %s:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
              ntohs(cliaddr.sin_port)); 
        while (1) {
            n = read(connfd, buf, MAXLINE);
            if (!strncmp(buf, "quit", 4)) {
                break;
            }
            write(1, buf, n);
            for (int i = 0; i < n; i++) {
                buf[i] = toupper(buf[i]);
            }
            write(connfd, buf, n);
        }
        close(connfd);
    }
    return 0;
}
