cmake_minimum_required(VERSION 3.28)
project(TPEngine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SERVER_SOURCES
        src/main.cpp
        src/http/*.cpp
        src/thread_pool/*.cpp
	src/epoll/*.cpp
)

add_executable(TPEngine ${SERVER_SOURCES})
