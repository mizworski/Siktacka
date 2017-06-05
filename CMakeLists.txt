cmake_minimum_required(VERSION 3.5)
project(sik)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
#set(CMAKE_CXX_FLAGS         "-Werror -Wall -std=c++14")
#set(CMAKE_CXX_FLAGS         "-lboost_system -lboost_date_time -lboost_thread -lboost_crc -std=c++14")
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES HelperFunctions.cc HelperFunctions.h GameClient.cc GameClient.h ServerMessage.cc ServerMessage.h ClientMessage.cc ClientMessage.h GameServer.cc GameServer.h)

add_executable(client ${SOURCE_FILES} client.cc)
add_executable(server ${SOURCE_FILES} server.cc)
add_executable(message_tests ${SOURCE_FILES} Tests/ServerEventsTests.cc)
add_executable(client_message_test ${SOURCE_FILES} Tests/ClientMessageTest.cc)