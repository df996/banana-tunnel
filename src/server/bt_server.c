//
// Created by cheng on 2023/5/30.
//

#include <stdio.h>
#include "bt_epoll.h"

#define PORT "3101"

int main(int argc, char* argv[]) {
    char *port = PORT;

    if (argc >= 2) {
        port = argv[1];
    }

    bt_epoll_start(port);
    return 0;
}
