#include <stdio.h>
#include "bt_epoll.h"

#define PORT "3101"

static void on_recv(void *args);

int main(int argc, char* argv[]) {
    char *port = PORT;

    if (argc >= 2) {
        port = argv[1];
    }
    
    struct bt_epoll *epoll = (struct bt_epoll *)
        malloc(sizeof(struct bt_epoll));

    int sfd = init_socket(port);
    init_epoll(epoll);

    printf("sfd: %d, epfd: %d\n", sfd, epoll->epfd);

    // for(;;) {}

    // init_epoll();

    return 0;
}