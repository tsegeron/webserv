cmake_minimum_required(VERSION 3.21)
project(zero_webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(zero_webserv
        srcs/main.cpp
        srcs/Utils.cpp
        srcs/Server.cpp
        srcs/SimpSocket.cpp
        hdrs/Utils.hpp
        hdrs/Server.hpp
        srcs/SimpSocket.hpp
        hdrs/IServer.hpp)
