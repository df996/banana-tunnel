//
// Created by cheng on 2023/5/30.
//

#include <stdio.h>
#include "bt_epoll.h"

#define PORT "3101"

static void on_recv(void *args);

int main(int argc, char* argv[]) {
    char *port = PORT;

    if (argc >= 2) {
        port = argv[1];
    }

    struct bt_epoll *bt_epoll = bt_epoll_create();
    bt_epoll->bind(port);
    bt_epoll->listen(BT_EPOLL_RECV, on_recv);
    bt_epoll->run();

    return 0;
}

static void on_recv(void *args) {
    printf("listener被调用");
}
