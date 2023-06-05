#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../util/sys_log.h"

/**
 * 读写缓冲区长度
 */
#define BT_EPOLL_BUFFER_LENGTH 4096

/**
 * epoll 最大事件数
 */
#define BT_EPOLL_MAX_EVENTS 1024

typedef int callback(int sfd, int events, void *arg);

/**
 * accept 回调
 */
typedef void (*fn_appect_cb)(void *args);

/**
 * send 回调
 */
typedef void (*fn_send_cb)(void *args);

/**
 * recv 回调
 */
typedef void (*fn_recv_cb)(void *args);

/**
 * bt_epoll 事件结构
 */
struct bt_epoll_event {
    int sfd;
    int events;
    void *arg;
    int (*callback)(int fd, int events, void *arg);
    int status;
    char buf[BT_EPOLL_BUFFER_LENGTH];
    int len;
    long last_active;


    int send_len;
    unsigned char *send_buf[BT_EPOLL_BUFFER_LENGTH];
    int recv_len;
    unsigned char *recv_buf[BT_EPOLL_BUFFER_LENGTH];

    fn_appect_cb appect_cb;
    fn_recv_cb recv_cb;
    fn_send_cb send_cb;
};

/**
 * bt_epoll 事件块状空间结构
 */
struct bt_epoll_event_block {
    struct bt_epoll_event_block *next;
    struct bt_epoll_event *events;
};

/**
 * bt_epoll 主结构
 */
struct bt_epoll {
    /**
     * epoll fd
     */
    int epfd;
    /**
     * block count
     */
    int blkcnt;

    struct bt_epoll_event_block *evblk;
};

/**
 * 创建socket
 * @param port 监听端口
 * @returns 执行成功返回socketfd，失败返回-1
 */
int create_socket(const char *port);

/**
 * 初始化bt_epoll
 * @param bt_epoll结构指针
 * @return 执行成功返回0, 失败返回-1
 */
int init_epoll(struct bt_epoll *epoll);

int epoll_add_listener(struct bt_epoll *epoll, int sfd);

int epoll_run(struct bt_epoll *epoll);