cmake_minimum_required(VERSION 3.22)
project(tcp_echo_server)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(setup)
include(dependencies)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)