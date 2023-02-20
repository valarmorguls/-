/*************************************************************************
	> File Name: reactor_mulcb.c
	> Author: 
	> Mail: 
	> Created Time: Mon 20 Feb 2023 02:34:17 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAXLINE 4096
#define BUFFER_LENGTH 1024
#define MAX_EPOLL_EVENT 1024
#define POLL_SIZE 1024
#define SERVER_PORT 8000

#define NOSET_CB 0
#define READ_CB 1
#define WRITE_CB 2
#define ACCEPT_CB 3

typedef int NCALLBACK(int fd, int event, void *arg);

struct nitem {
    int fd;
    int status;
    int events;
    void *arg;
#if  0
    NCALLBACK callback;
#else 
    NCALLBACK *readcb; // epollin
    NCALLBACK *writecb; // epollout
    NCALLBACK *acceptcb; // epollin
#endif
    unsigned char sbuffer[BUFFER_LENGTH];
    int slength;
    unsigned char rbuffer[BUFFER_LENGTH];
    int rlength;
};

//每一个块大小为1024
struct itemblock {
    struct nitem *items;    
    struct itemblock *next;
};

struct reactor {
    int epfd;
    struct itemblock *head;
};

int init_reactor(struct reactor *re);

int read_callback(int fd, int event, void *arg);

int write_callback(int fd, int event, void *arg);

int accept_callback(int fd, int event, void *arg);

struct reactor *instance = NULL;

//singleton
struct reactor *getInstance(void) {
    if (instance == NULL) {
        instance = (struct reactor *)malloc(sizeof(struct reactor));
        if (instance == NULL) return NULL;
        memset(instance, 0, sizeof(struct reactor));

        if (init_reactor(instance) < 0) {
            free(instance);
            return NULL;
        }
    } 
    return instance;
}

int init_server(int port) {
    int listenfd;
    struct sockaddr_in serveraddr;
    char buff[MAXLINE];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    if (listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    return listenfd;
}

int init_reactor(struct reactor *r) {
    if (r == NULL) return -1;
    int epfd = epoll_create(1);
    r->epfd = epfd;

    r->head = (struct itemblock *)malloc(sizeof(struct itemblock));
    if (r->head == NULL) {
        close(epfd);
        return -2;
    }
    memset(r->head, 0, sizeof(struct itemblock));
    r->head->items = malloc(sizeof(MAX_EPOLL_EVENT * sizeof(struct nitem)));
    if (r->head->items == NULL) {
        free(r->head);
        close(epfd);
        return -2;
    }
    memset(r->head->items, 0, sizeof(MAX_EPOLL_EVENT * sizeof(struct nitem)));
    r->head->next = NULL;
    return 0;
}

int nreactor_set_event(int fd, NCALLBACK cb, int event, void *arg) {
    struct reactor *r = getInstance();
    struct epoll_event ev = {0};
    if (event == READ_CB) {
        r->head->items[fd].fd = fd;
        r->head->items[fd].readcb = cb;
        r->head->items[fd].arg = arg;
        ev.events = EPOLLIN;
    } else if (event == WRITE_CB) {
        r->head->items[fd].fd = fd;
        r->head->items[fd].writecb = cb;
        r->head->items[fd].arg = arg;
        ev.events = EPOLLOUT;
    } else if (event == ACCEPT_CB) {
        r->head->items[fd].fd = fd;
        r->head->items[fd].acceptcb = cb;
        r->head->items[fd].arg = arg;
        ev.events = EPOLLIN;
    }
    ev.data.ptr = &r->head->items[fd];

    if (r->head->items[fd].events == NOSET_CB) {
        if (epoll_ctl(r->epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
            printf("epoll_ctl EPOLL_CTL_ADD failed, %d\n", errno);
            return -1;
        }
        r->head->items[fd].events = event;
    } else if (r->head->items[fd].events != event) {
        if (epoll_ctl(r->epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
            printf("epoll_ctl EPOLL_CTL_MOD failed\n");
            return -1;
        }
        r->head->items[fd].events = event;
    }
    return 0;
}

int nreactor_del_event(int fd, NCALLBACK cb, int event, void *arg) {
    struct reactor *r = getInstance();
    struct epoll_event ev = {0};
    ev.data.ptr = arg;
    epoll_ctl(r->epfd, EPOLL_CTL_DEL, fd, &ev);
    r->head->items[fd].events = 0;
    return 0;
}

int read_callback(int fd, int event, void *arg) {
    struct reactor *R = getInstance();
    unsigned char *buffer = R->head->items[fd].rbuffer;
   
#if 0 // ET
    int idx = 0, ret = 0;
    while (idx < BUFFER_LENGTH) {
        ret = recv(fd, buffer + idx, BUFFER_LENGTH - idx, 0);
        if (ret == -1) {
            break;
        } else if (ret > 0) {
            idx += ret;
        } else {
            break;
        }
    }
    if (idx == BUFFER_LENGTH && ret == -1) {
        nreactor_set_event(fd, read_callback, READ_CB, NULL);
    } else if (ret == 0) {

    } else {
        nreactor_set_event(fd, write_callback, WRITE_CB, NULL);
    }
#else //LT 
    int ret = recv(fd, buffer, BUFFER_LENGTH, 0);
    if (ret == 0) {
        nreactor_del_event(fd, NULL, 0, NULL);
        close(fd);
    } else if (ret > 0) {
        unsigned char *sbuffer = R->head->items[fd].sbuffer;
        memcpy(sbuffer, buffer, ret);
        R->head->items[fd].slength = ret;
        printf("readcb: %s\n", sbuffer);
        nreactor_set_event(fd, write_callback, WRITE_CB, NULL);
    }
#endif 
}

int write_callback(int fd, int event, void *arg) {
    struct reactor *R = getInstance();
    unsigned char *sbuffer = R->head->items[fd].sbuffer;
    int length = R->head->items[fd].slength;
    int ret = send(fd, sbuffer, length, 0);
    if (ret < length) {
        nreactor_set_event(fd, write_callback, WRITE_CB, NULL);
    } else {
        nreactor_set_event(fd, read_callback, READ_CB, NULL);
    }
    return 0;
}

int accept_callback(int fd, int event, void *arg) {
    int connfd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    if ((connfd = accept(fd, (struct sockaddr *)&client, &len)) == -1) {
        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    nreactor_set_event(connfd, read_callback, READ_CB, NULL);
}

int reactor_loop(int listenfd) {
    struct reactor *R = getInstance();
    struct epoll_event events[POLL_SIZE] = {0};
    while (1) {
        int nready = epoll_wait(R->epfd, events, POLL_SIZE, 5);
        if (nready == -1) {
            continue;
        }
        int i = 0;
        for (int i = 0; i < nready; i++) {
            struct nitem *item = (struct nitem *)events[i].data.ptr;
            int connfd = item->fd;
            if (connfd == listenfd) {
                item->acceptcb(listenfd, 0, NULL);
            } else {
                if (events[i].events & EPOLLIN) {
                    item->readcb(connfd, 0, NULL);
                }
                if (events[i].events & EPOLLOUT) {
                    item->writecb(connfd, 0, NULL);
                }
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int connfd, n;
    int listenfd = init_server(SERVER_PORT);
    nreactor_set_event(listenfd, accept_callback, ACCEPT_CB, NULL);
    reactor_loop(listenfd);
    return 0;
}

