//
// Created by cheng on 2023/6/8.
//

#include "str_help.h"

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

    char *ptr, *val;

    val = strtok_s(str, ":", &ptr);
    ip_port->ip = (char *) malloc(strlen(val) + 1);
    strcpy_s(ip_port->ip, strlen(val) + 1, val);

    val = strtok_s(NULL, ":", &ptr);
    ip_port->port = (char *) malloc(strlen(val) + 1);
    strcpy_s(ip_port->port, strlen(val) + 1, val);

    return ip_port;
}

/**
 * 释放ip_port结构
 * @param ip_port 结构
 */
void free_ip_port(struct ip_port *ip_port) {
    if (ip_port == NULL) {
        return;
    }

    if (ip_port->ip != NULL) {
        free(ip_port->ip);
    }

    if (ip_port->port != NULL) {
        free(ip_port->port);
    }

    free(ip_port);
}