/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Wed 22 Dec 2021 09:03:19 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#define SERV_PORT 8000
#define MAXLINE 80
#define prrexit(msg) {\
    perror(msg);\
    exit(EXIT_FAILURE);\
}
int main(void) {
    struct sockaddr_in serveraddr, cliaddr;
    int listenfd, connfd;
    socklen_t cliaddr_len; 
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int n, i;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
        prrexit("socket");
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        prrexit("bind");
    if (listen(listenfd, 3) < 0)
        prrexit("listen");
    printf("Accepting connnections...\n");
    while (1) {
        cliaddr_len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len);
        if (connfd < 0)
            prrexit("accept");
        printf("received from %s:%d\n",\
              inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
        pid_t pid = fork();
        if (pid < 0)
            prrexit("fork");
        //父进程等待，创建链接
        if (pid > 0) {
            close(connfd);
            while (waitpid(-1, NULL, WNOHANG)) {}
            continue;
        }
        close(listenfd);
        while (1) {
            n = read(connfd, buf, MAXLINE);
            if (!strncmp(buf, "quit", 4))
                break;
            write(1, buf, n);
            for (i = 0; i < n; i++)
                buf[i] = toupper(buf[i]);
            write(connfd, buf, n);
        }
        close(connfd);
    }
    return 0;
}
