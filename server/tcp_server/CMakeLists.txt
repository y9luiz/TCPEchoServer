
if (WIN32)
  add_definitions(-D_WIN32_WINNT=0x0601)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_DEBUG_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED system)

add_library(tcp_server tcp_echo_server.cpp)

target_include_directories(tcp_server 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(tcp_server 
    PRIVATE
        Boost::system
)
