//
// Created by cheng on 2023/6/8.
//

#ifndef BANANA_TUNNEL_STR_HELP_H
#define BANANA_TUNNEL_STR_HELP_H
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
struct ip_port *split_ip_port(char *str);

/**
 * 释放ip_port结构
 * @param ip_port 结构
 */
void free_ip_port(struct ip_port *ip_port);
#endif //BANANA_TUNNEL_STR_HELP_H
