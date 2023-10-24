cmake_minimum_required(VERSION 3.26)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled follower.cpp
        client.cpp
        leader.cpp
        info.cpp)

target_link_libraries(untitled ws2_32)
