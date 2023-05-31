/*------------------------------------------------------
¦ 文件名
¦ 文件描述
¦ 
¦ Author: 大风
¦ Email: 1236192@qq.com
¦ Date: 2023-05-30 16:52:34
¦ Version: 1.0
¦ FilePath: src/server/socket/bt_epoll.c
¦------------------------------------------------------*/

#include "bt_epoll.h"

struct bt_epoll_info {
    int sfd;
    int epfd;
    int flags;
    const char *port;
    struct epoll_event event;
    struct epoll_event *events;
};

void init_socket(struct bt_epoll_info *epoll_info) {
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(NULL, epoll_info->port, &hints, &result) != 0) {
        printf("无法获取到地址信息\n");
        exit(1);
    }

    while(NULL != result) {
        if ((epoll_info->sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
            result = result->ai_next;
            continue;
        }

        if (bind(epoll_info->sfd, result->ai_addr, result->ai_addrlen) == -1) {
            printf("端口:%s绑定失败，请检查端口是否被占用\n", epoll_info->port);
            exit(1);
        }

        if (listen(epoll_info->sfd, SOMAXCONN) == -1) {
            printf("监听端口:%s失败\n", epoll_info->port);
            exit(1);
        }

        freeaddrinfo(result);
        sys_log(INFO, "监听端口:%s成功", epoll_info->port);
        return;
    }

    printf("无法预料的异常");
    exit(1);
}

void init_epoll(struct bt_epoll_info *epoll_info) {
    do {
        if ((epoll_info->flags = fcntl(epoll_info->sfd, F_GETFL, 0)) == -1)
            break;

        epoll_info->flags |= O_NONBLOCK;

        if (fcntl(epoll_info->sfd, F_SETFL, epoll_info->flags) == -1)
            break;

        if ((epoll_info->epfd = epoll_create1(0)) == -1)
            break;

        epoll_info->event.data.fd = epoll_info->sfd;
        epoll_info->event.events = EPOLLIN | EPOLLET;

        if (epoll_ctl(epoll_info->epfd, EPOLL_CTL_ADD, epoll_info->sfd, &epoll_info->event) == -1)
            break;

        epoll_info->events = calloc(1024, sizeof(epoll_info->event));

        return;
    } while(1);

    printf("%s", strerror(errno));
    exit(1);
}

void bt_epoll_start(const char *port) {
    int epct;
    struct bt_epoll_info epoll_info;

    epoll_info.port = port;
    init_socket(&epoll_info);
    init_epoll(&epoll_info);

    for (;;) {
        epct = epoll_wait(epoll_info.epfd, epoll_info.events, 1024, -1);
        printf("%d\n", epct);
    }
    return;
}
