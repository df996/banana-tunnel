/*------------------------------------------------------
¦ 文件名
¦ 文件描述
¦ 
¦ Author: 大风
¦ Email: 1236192@qq.com
¦ Date: 2023-05-30 16:55:24
¦ Version: 1.0
¦ FilePath: src/server/socket/bt_epoll.h
¦------------------------------------------------------*/

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include "../util/sys_log.h"

struct bt_epoll_info {
    int sfd;
    int epfd;
    int flags;
    const char *port;
    struct epoll_event event;
    struct epoll_event *events;
};

/**
 * 启动
 */
void bt_epoll_start(const char *port);
