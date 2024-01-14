
add_executable(client_example client_example.cpp)

target_include_directories(client_example 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)


target_link_libraries(client_example 
    PRIVATE
        Boost::system
        Messages
        CypherUtils
)