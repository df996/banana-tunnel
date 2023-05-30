cmake_minimum_required(VERSION 3.19)
project(banana_tunnel_client C)

set(CMAKE_C_STANDARD 11)

add_executable(
        bt_client
        bt_client.c
)