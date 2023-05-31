//
// Created by cheng on 2023/5/30.
//

#include <stdio.h>
#include "bt_epoll.h"

int main(int argc, char* argv[]) {
    int a = bt_create_sfd("8081");
    printf("%d\n", a);
    for (;;) {
    }
    return 0;
}
