//
// Created by cheng on 2023/6/9.
//

#include "bt_conn.h"

SOCKET bt_connet(char *srv_ip, char *port) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        return 0;
    }

    SOCKET c_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_sock == INVALID_SOCKET) {
        return 0;
    }

    struct sockaddr_in s_in;
    s_in.sin_family = AF_INET;
    s_in.sin_port = htons(atoi(port));
    inet_pton(AF_INET, srv_ip, &s_in.sin_addr.S_un.S_addr);
    if (connect(c_sock, (SOCKADDR *)&s_in, sizeof(s_in)) == SOCKET_ERROR) {
        sys_log(LOG_ERROR, "error: connection %s:%s failed", srv_ip, port);
        return 0;
    }

    return 0;
}