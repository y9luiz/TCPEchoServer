add_library(Messages STATIC messages.cpp)
target_include_directories(Messages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})