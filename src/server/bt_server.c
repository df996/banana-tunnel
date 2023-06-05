#include "bt_epoll.h"

#define PORT "3101"

static void on_recv(void *args);

int main(int argc, char* argv[]) {
    char *port = PORT;

    if (argc >= 2) {
        port = argv[1];
    }

    int p = atoi(port);
    
    struct bt_epoll *epoll = (struct bt_epoll *)
        malloc(sizeof(struct bt_epoll));
    init_epoll(epoll);

    int i;
    for (i = 0; i < 1; i++) {
        char sp[2] = { 0 };
        sprintf(sp, "%d", p + i);
        int sfd = create_socket(sp);
        epoll_add_listener(epoll, sfd);
        printf("sfd: %d, epfd: %d, port: %s\n", sfd, epoll->epfd, sp);
    }
    epoll_run(epoll);

    return 0;
}