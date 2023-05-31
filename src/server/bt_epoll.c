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

int bt_create_sfd(const char *port) {
    struct addrinfo hints;
    struct addrinfo *result;
    int err, sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((err = getaddrinfo(NULL, port, &hints, &result)) != 0) {
        sys_log(FATAL, "getaddrinfo: %s", gai_strerror(err));
        return -1;
    }

    struct addrinfo *rp = result;
    while (rp != NULL) {
        if ((sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            rp = rp->ai_next;
            continue;
        }

        if ((err = bind(sfd, rp->ai_addr, rp->ai_addrlen)) == 0) {
            break;
        }

        sys_log(FATAL, "bind: %s", gai_strerror(err));
        exit(1);
    }

    if (NULL == rp) {
        sys_log(ERROR, "could not bind 8080");
        return -1;
    }

    freeaddrinfo(result);
    return sfd;
}

int aaabt_create_socket() {
    char hostname[100];
    struct addrinfo hints;
    struct addrinfo *result;
    int err, sfd;

    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("gethostname");
        return -2;
    }

    printf("hostname: %s\n", hostname);

    memset(&hints, 0, sizeof(struct addrinfo));
    // hints.ai_family = AF_UNSPEC;
    // hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;
    if ((err = getaddrinfo(hostname, "8080", &hints, &result)) != 0) {
        sys_log(FATAL, "getaddrinfo: %s", gai_strerror(err));
        return -2;
    }

    struct addrinfo *current;
    struct sockaddr_in *sin;
    current = result;
    while (current && current->ai_canonname) {
        sin = (struct sockaddr_in*) current->ai_addr;
        sys_log(INFO, "name:%s, ip: %s",
                current->ai_canonname,
                inet_ntop(AF_INET, &sin->sin_addr.s_addr, hostname, sizeof(hostname)));
        current = current->ai_next;
    }

    return -1;
}
