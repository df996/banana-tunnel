//
// Created by cheng on 2023/6/9.
//

#ifndef BANANA_TUNNEL_BT_PROTOCOL_H
#define BANANA_TUNNEL_BT_PROTOCOL_H

struct bt_protocol {
    char *cmd;
    char *payload;
};

struct bt_payload {
    char *port;
    char *content;
};

#endif //BANANA_TUNNEL_BT_PROTOCOL_H
