//
// Created by cheng on 2023/5/30.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * ip端口信息
 */
struct ip_port {
    char *ip;
    char *port;
};

/**
 * 分割ip:port字符串
 * @param str
 * @return 返回ip_port结构体, 如果格式错误返回NULL
 */
struct ip_port *split_ip_port(char *str) {
    struct ip_port *ip_port = (struct ip_port *) (struct srv_ip_port *)
            malloc(sizeof(struct ip_port));

    char *index = strchr(str, ':');
    if (index == NULL) {
        return NULL;
    }

    return ip_port;
}

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

    // char *index = strchr(argv[1], ':');
    // if (index == NULL) {
    //     printf("Usage: bt-client server:port ports...\n");
    //     exit(1);
    // }

    // char *srv_ip, *srv_port, *ptr;
    // char *val = strtok_s(argv[1], ":", &ptr);
    // srv_ip = (char *) malloc(strlen(val) + 1);
    // strcpy_s(srv_ip, strlen(val) + 1, val);
    // val = strtok_s(NULL, ":", &ptr);
    // srv_port = (char *) malloc(strlen(val) + 1);
    // strcpy_s(srv_port, strlen(val) + 1, val);

    // printf("IP: %s, PORT: %s", srv_ip, srv_port);
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