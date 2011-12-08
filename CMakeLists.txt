cmake_minimum_required(VERSION 2.8)
project(spacepart C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
