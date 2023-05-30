cmake_minimum_required(VERSION 3.19)
project(banana_tunnel C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(${PROJECT_SOURCE_DIR}/src/client)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/server)