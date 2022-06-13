cmake_minimum_required(VERSION 3.21)
project(zero_webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(zero_webserv
        srcs/main.cpp
        srcs/Utils.cpp
        srcs/Server.cpp
        srcs/SimpSocket.cpp
        srcs/Request.cpp
        srcs/Config.cpp
        srcs/Urls.cpp
        srcs/Response.cpp
        hdrs/Utils.hpp
        hdrs/Server.hpp
        hdrs/SimpSocket.hpp
        hdrs/IServer.hpp
        hdrs/Request.hpp
        hdrs/Config.hpp
        hdrs/Urls.hpp
        hdrs/ConfigParams.hpp
        hdrs/Response.hpp)
