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

typedef enum BT_EPOLL_EVENT_TYPE {
    BT_EPOLL_ACCEPT,
    BT_EPOLL_SEND,
    BT_EPOLL_RECV
} BT_EPOLL_EVENT_TYPE;

typedef void (*fn_bind)(const char *port);
typedef void (*fn_listen_cb)(void * args);
typedef void (*fn_listen)(BT_EPOLL_EVENT_TYPE event_type, fn_listen_cb cb);
typedef void (*fn_start)();

struct bt_epoll {
    const char *port;

    fn_bind bind;
    fn_listen listen;
    fn_start run;
};

struct bt_epoll_info {
    int sfd;
    int epfd;
    int flags;
    const char *port;
    struct epoll_event event;
    struct epoll_event *events;
};

/**
 * 创建bt epoll
 */
struct bt_epoll *bt_epoll_create();
