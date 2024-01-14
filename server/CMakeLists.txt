add_executable(echo_server main.cpp)

add_subdirectory(tcp_server)

target_link_libraries(echo_server PRIVATE tcp_server)