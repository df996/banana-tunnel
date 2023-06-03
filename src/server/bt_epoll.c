#include "bt_epoll.h"

static int epoll_alloc(struct bt_epoll *epoll);
static struct bt_epoll_event *epoll_index(struct bt_epoll *epoll, int sfd);

/**
 * 初始化socket
 * @param port 监听端口
 * @returns 执行成功返回socketfd，失败返回-1
 */
int init_socket(const char *port) {
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
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

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

    // 如果block数量小余等于索引，则暴力分配
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