#include <stdio.h>
#include "bt_epoll.h"

#define PORT "3101"

static void on_recv(void *args);

int main(int argc, char* argv[]) {
    char *port = PORT;

    if (argc >= 2) {
        port = argv[1];
    }
    
    // init_socket(port);

    struct bt_epoll *epoll = (struct bt_epoll *)
        malloc(sizeof(struct bt_epoll));

    init_epoll(epoll);

    // for(;;) {}

    // init_epoll();

    return 0;
}