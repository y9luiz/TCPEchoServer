add_executable(server main.cpp)

add_subdirectory(tcp_server)

target_link_libraries(server PRIVATE tcp_server)