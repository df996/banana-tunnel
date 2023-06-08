cmake_minimum_required(VERSION 3.19)
project(banana_tunnel_client C)

set(CMAKE_C_STANDARD 11)

add_executable(
        bt-client
        bt_client.c
        ../util/str_help.h
        ../util/str_help.c
)
