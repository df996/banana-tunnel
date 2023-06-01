cmake_minimum_required(VERSION 3.19)
project(banana_tunnel_server C)

set(CMAKE_C_STANDARD 11)

add_executable(
        bt-server
        bt_server.c
        bt_epoll.h
        bt_epoll.c
        bt_epoll_event.h
        bt_epoll_event.c
        ../util/sys_log.h
        ../util/sys_log.c
)
