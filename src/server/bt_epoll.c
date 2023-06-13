#include "bt_epoll.h"

static int epoll_alloc(struct bt_epoll *epoll);
static struct bt_epoll_event *epoll_index(struct bt_epoll *epoll, int sfd);
static int bt_send(int sfd, int events, void *arg);
static int bt_recv(int sfd, int events, void *arg);
static int bt_accept(int sfd, int events, void *arg);
static int add_event(int epfd, int events, struct bt_epoll_event *ev);
static int remove_event(int epfd, struct bt_epoll_event *ev);
static void set_event(struct bt_epoll_event *ev, int sfd, callback cb, void *arg);

/**
 * 创建socket
 * @param port 监听端口
 * @returns 执行成功返回socketfd，失败返回-1
 */
int create_socket(const char *port) {
    int sfd, flags;

    // 创建socket
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        sys_log(LOG_FATAL, "errno: %s, socket() failed", strerror(errno));
        return -1;
    }

    // 获取到sfd的文件状态标志位
    if ((flags = fcntl(sfd, F_GETFL, 0)) == -1) {
        sys_log(LOG_FATAL, "errno: %s, fcntl() F_GETFL failed", strerror(errno));
        return -1;
    }

    // 追加非阻塞到标志位
    flags |= O_NONBLOCK;

    // 设置标志位到sfd的文件状态
    if (fcntl(sfd, F_SETFL, flags) == -1) {
        sys_log(LOG_FATAL, "errno: %s, fcntl() F_SETFL failed, flags: %d", 
            strerror(errno), flags);
        return -1;
    }

    // 构建服务器监听地址
    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_port = htons(atoi(port));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    // 设置socket，允许地址被复用
    // 许服务器bind一个地址，即使这个地址当前已经存在已建立的连接，比如：
    // 服务器启动后，有客户端连接并已建立，如果服务器主动关闭，那么和客户端的连接会处于
    // TIME_WAIT状态，此时再次启动服务器，就会bind不成功，报：Address already in use。
    // 服务器父进程监听客户端，当和客户端建立链接后，fork一个子进程专门处理客户端的请求，如
    // 果父进程停止，因为子进程还和客户端有连接，所以再次启动父进程，也会报Address already in use。
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // socket 绑定到地址
    if (bind(sfd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0) {
        sys_log(LOG_FATAL, "errno: %s, bind() failed", strerror(errno));
        return -1;
    }

    // 开始监听
    if (listen(sfd, SOMAXCONN) < 0) {
        sys_log(LOG_FATAL, "errno: %s, listen, listen() failed", strerror(errno));
        return -1;
    }

    return sfd;
}

/**
 * 初始化bt_epoll
 * @param epoll bt_epoll结构体指针
 * @return 执行成功返回0, 失败返回-1
 */
int init_epoll(struct bt_epoll *epoll) {
    if (epoll == NULL) {
        return -1;
    }

    // 创建epoll
    memset(epoll, 0, sizeof(struct bt_epoll));
    epoll->epfd = epoll_create1(0);
    if ((epoll->epfd = epoll_create1(0)) <= 0) {
        sys_log(LOG_FATAL, "errno: %s, epoll_create1 failed", strerror(errno));
        return -1;
    }

    // 申请events内存空间
    struct bt_epoll_event *evs = (struct bt_epoll_event *)
        malloc((BT_EPOLL_MAX_EVENTS) * sizeof(struct bt_epoll_event));
    if (evs == NULL) {
        sys_log(LOG_FATAL, "error: malloc(), alloc bt_epoll events failed");
        return -1;
    }
    memset(evs, 0, (BT_EPOLL_MAX_EVENTS) * sizeof(struct bt_epoll_event));
    
    // 申请block空间
    struct bt_epoll_event_block *evblk = (struct bt_epoll_event_block *)
        malloc(sizeof(struct bt_epoll_event_block));
    if (evblk == NULL) {
        sys_log(LOG_FATAL, "error: malloc(), alloc bt_epoll event block failed");
        return -1;
    }
    memset(evblk, 0, sizeof(struct bt_epoll_event_block));
    
    // 初始化块状链表
    evblk->next = NULL;
    evblk->events = evs;

    // 设置bt_epoll
    epoll->blkcnt = 1;
    epoll->evblk = evblk;
    
    return 0;
}

/**
 * 分配资源
 * @param epoll bt_epoll结构体指针
 * @return 执行成功返回0, 失败返回-1
 */
static int epoll_alloc(struct bt_epoll *epoll) {
    if (epoll == NULL) {
        return -1;
    }

    if (epoll->evblk == NULL) {
        return -1;
    }

    // 获取块状链表中最后一个节点
    struct bt_epoll_event_block *last_evblk = epoll->evblk;
    while (last_evblk != NULL) {
        last_evblk = last_evblk->next;
    }

    // 申请events内存空间
    struct bt_epoll_event *evs = (struct bt_epoll_event *)
        malloc(BT_EPOLL_MAX_EVENTS * sizeof(struct bt_epoll_event));
    if (evs == NULL) {
        sys_log(LOG_FATAL, "error: malloc(), alloc bt_epoll events failed");
        return -1;
    }
    memset(evs, 0, BT_EPOLL_MAX_EVENTS * sizeof(struct bt_epoll_event));

    // 申请block空间
    struct bt_epoll_event_block *evblk = (struct bt_epoll_event_block *)
        malloc(sizeof(struct bt_epoll_event_block));
    if (evblk == NULL) {
        sys_log(LOG_FATAL, "error: malloc(), alloc bt_epoll event block failed");
        return -1;
    }
    memset(evblk, 0, sizeof(struct bt_epoll_event_block));

    // 初始化块状链表
    evblk->next = NULL;
    evblk->events = evs;

    // 重置指向
    last_evblk->next = evblk;
    epoll->blkcnt++;

    return 0;
}

static struct bt_epoll_event *epoll_index(struct bt_epoll *epoll, int sfd) {
    // 获取当前socket fd获取对应events所在的block
    int blk_idx = sfd / BT_EPOLL_MAX_EVENTS;

    // 如果block数量小余等于索引，则分配
    while (blk_idx >= epoll->blkcnt) {
        epoll_alloc(epoll);
    }

    int i = 0;
    struct bt_epoll_event_block *blk = epoll->evblk;
    while (i++ < blk_idx && blk != NULL) {
        blk = blk->next;
    }

    return &blk->events[sfd % BT_EPOLL_MAX_EVENTS];
}

static void set_event(struct bt_epoll_event *ev, int sfd, callback cb, void *arg) {
    ev->sfd = sfd;
    ev->callback = cb;
    ev->events = 0;
    ev->arg = arg;
    ev->last_active = time(NULL);
}

static int add_event(int epfd, int events, struct bt_epoll_event *ev) {
    // 内核event
    struct epoll_event ep_ev = {0, {0}};

    ep_ev.data.ptr = ev;
    ep_ev.events = ev->events = events;

    int op;
    if (ev->status == 1) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if (epoll_ctl(epfd, op, ev->sfd, &ep_ev) < 0) {
        sys_log(LOG_FATAL, "event add failed [sfd=%d], events[%d]", ev->sfd, events);
        return -1;
    }

    return 0;
}

static int remove_event(int epfd, struct bt_epoll_event *ev) {
    struct epoll_event event = {0, {0}};
    if (ev->status != 1) {
        return -1;
    }

    event.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->sfd, &event);
    return 0;
}

static int bt_send(int sfd, int events, void *arg) {
    struct bt_epoll *epoll = (struct bt_epoll *)arg;
    struct bt_epoll_event *ev = epoll_index(epoll, sfd);
    char *buf = (char *)malloc(BT_EPOLL_BUFFER_LENGTH);
    memset(buf, 0, BT_EPOLL_BUFFER_LENGTH);
    strcpy(buf, "hello client !");
    int len = send(sfd, (void *)buf, BT_EPOLL_BUFFER_LENGTH, 0);
    if (len > 0) {
        sys_log(LOG_INFO, "send[fd=%d], [%d]%s", sfd, len, ev->buf);
        remove_event(epoll->epfd, ev);
        set_event(ev, sfd, bt_recv, epoll);
        add_event(epoll->epfd, EPOLLIN, ev);
    } else {
        close(ev->sfd);
        remove_event(epoll->epfd, ev);
        sys_log(LOG_ERROR, "send[fd=%d] error %s", sfd, strerror(errno));
    }
    free(buf);
    return len;
}

static int bt_recv(int sfd, int events, void *arg) {
    struct bt_epoll *epoll = (struct bt_epoll *)arg;
    struct bt_epoll_event *ev = epoll_index(epoll, sfd);
    int len = recv(sfd, ev->buf, BT_EPOLL_BUFFER_LENGTH, 0);
    remove_event(epoll->epfd, &epoll->evblk->events[sfd]);
    if (len > 0) {
        ev->len = len;
        ev->buf[len] = '\0';
        sys_log(LOG_INFO, "recv[fd=%d], [%d]%s", sfd, len, ev->buf);
        set_event(ev, sfd, bt_send, epoll);
        add_event(epoll->epfd, EPOLLOUT, ev);
    } else if (len == 0) {
        close(ev->sfd);
        sys_log(LOG_INFO, "[fd=%d] closed", sfd);
    } else {
        close(ev->sfd);
        sys_log(LOG_ERROR, "recv[fd=%d] error[%d]:%s", sfd, errno, strerror(errno));
    }
    return len;
}

static int bt_accept(int sfd, int events, void *arg) {
    struct bt_epoll *epoll = (struct bt_epoll *)arg;
    if (epoll == NULL) {
        return -1;
    }

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int client_fd;
    if ((client_fd = accept(sfd, (struct sockaddr *) &client_addr, &len)) == -1) {
        if (errno != EAGAIN && errno != EINTR) {
            sys_log(LOG_ERROR, "error accept: %s", strerror(errno));
            return -1;
        }
    }

    int flag = 0;
    if ((flag = fcntl(client_fd, F_SETFL, O_NONBLOCK)) < 0) {
        sys_log(LOG_ERROR, "fcntl nonblocking failed");
        return -1;
    }
    
    struct bt_epoll_event *ev = epoll_index(epoll, client_fd);
    set_event(ev, client_fd, bt_recv, epoll);
    add_event(epoll->epfd, EPOLLIN, ev);
    sys_log(LOG_INFO, "new connect [%s:%d], pos[%d]",
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);
    return 0;
}

int epoll_add_listener(struct bt_epoll *epoll, int sfd) {
    if (epoll == NULL || epoll->evblk == NULL) {
        return -1;
    }

    struct bt_epoll_event *ev = epoll_index(epoll, sfd);
    set_event(ev, sfd, bt_accept, epoll);
    add_event(epoll->epfd, EPOLLIN, ev);
    return 0;
}

int epoll_run(struct bt_epoll *epoll) {
    if (epoll == NULL || epoll->epfd < 0 || epoll->evblk == NULL) {
        return -1;
    }

    int i;
    struct epoll_event events[BT_EPOLL_MAX_EVENTS + 1];
    for (;;) {
        int nready = epoll_wait(epoll->epfd, events, BT_EPOLL_MAX_EVENTS, 1000);
        if (nready < 0) {
            sys_log(LOG_FATAL, "error: epoll_wait error");
            continue;
        }

        for (i = 0; i < nready; i++) {
            struct bt_epoll_event *ev = (struct bt_epoll_event *)events[i].data.ptr;
            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
                ev->callback(ev->sfd, events[i].events, ev->arg);
            }
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
                ev->callback(ev->sfd, events[i].events, ev->arg);
            }
        }
    }

    return 0;
}