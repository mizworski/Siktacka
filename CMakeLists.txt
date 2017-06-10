cmake_minimum_required(VERSION 3.5)
project(sik)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-Werror -Wall -std=c++14")
set(CMAKE_CXX_STANDARD 14)
set(SERVER_SRCS
        server.cc
        Game/GameServer.cc Game/GameServer.h)
set(CLIENT_SRCS
        client.cc
        Game/GameClient.cc Game/GameClient.h
        Network/TcpSocket.cc Network/TcpSocket.h Network/PollSocketsClient.cc Network/PollSocketsClient.h)
set(SOURCE_FILES
        Messages/ServerMessage.cc Messages/ServerMessage.h
        Messages/ClientMessage.cc Messages/ClientMessage.h
        Network/HelperFunctions.cc Network/HelperFunctions.h
        Network/UdpSocket.cc Network/UdpSocket.h
        Network/NetworkAddress.cc Network/NetworkAddress.h
        Network/PollSocketsServer.cc Network/PollSocketsServer.h
        )

add_executable(client ${SOURCE_FILES} ${CLIENT_SRCS})
add_executable(server ${SOURCE_FILES} ${SERVER_SRCS})