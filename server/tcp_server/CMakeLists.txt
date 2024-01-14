add_library(tcp_server
    STATIC
        tcp_echo_server.cpp
        connection/connection.cpp
)

target_include_directories(tcp_server 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(tcp_server 
    PRIVATE
        Boost::system
        Messages
        CypherUtils
)
