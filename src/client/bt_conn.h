//
// Created by cheng on 2023/6/9.
//

#ifndef BANANA_TUNNEL_BT_CONN_H
#define BANANA_TUNNEL_BT_CONN_H

#include <stdlib.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <WinSock2.h>
#include "../util/sys_log.h"

#pragma comment(lib, "ws2_32.lib")

SOCKET bt_connet(char *srv_ip, char *port);
#endif //BANANA_TUNNEL_BT_CONN_H
