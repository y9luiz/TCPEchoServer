if (WIN32)
  add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Put the binaries inside of the build folder
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
