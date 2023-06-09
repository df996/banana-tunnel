//
// Created by cheng on 2023/5/30.
//

#include <stdio.h>
#include "../util/str_help.h"

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("Usage: bt-client server:port ports...\n");
        exit(1);
    }

    struct ip_port *srv_ip_port = split_ip_port(argv[1]);
    if (srv_ip_port == NULL) {
        printf("Usage: bt-client server:port ports...\n");
        exit(1);
    }

    printf("ip: %s, port: %s\n", srv_ip_port->ip, srv_ip_port->port);
    printf("%s", argv[2]);
    free_ip_port(srv_ip_port);
    return 0;

    // if (argc <= 2) {
    //     printf("Usage: bt-client server-ip:port ports...\n");
    //     exit(1);
    // }

    // int index = 0;
    // char **arg = argv;
    // while (*arg != NULL) {
    //     index++;
    //     *arg++;
    //     if (index <= 2) {
    //         continue;
    //     }
    //     printf("%llu\n", strlen(*arg));
    //     printf("%s, %p\n", *arg, arg);
    // }

    // int count = 0; 
    // while(argv[++count] != NULL);
    // printf("argv length: %d\n", count);
    // printf("argv length: %d\n", sizeof(argv) / sizeof(*argv));

    // char **ports;
    // ports = malloc(sizeof(char *) * )

    // int index = 0;
    // char **arg = NULL;
    // for (arg = argv; *arg != NULL; arg++) {
    //     index++;
    //     if (index < 2) {
    //         continue;
    //     }
    //     printf("string: %s length: %d pinter: %llu\n", *arg, strlen(*arg), arg);
    // }

    return 0;
}