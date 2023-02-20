/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Fri 17 Feb 2023 01:59:54 PM CST
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERV_PORT 8000
#define MAXLINE 80

int main() {
    struct sockaddr_in serveraddr;
    char buf[MAXLINE];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
    connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    int n;
    while (n = read(0, buf, MAXLINE)) {
        write(sockfd, buf, n);
        if (!strncmp(buf, "quit", 4)) {
            break;
        }
        n = read(sockfd, buf, MAXLINE);
        printf("response from server:\n");
        write(1, buf, n);
    }
    close(sockfd);
    return 0;
}

