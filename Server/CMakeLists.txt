cmake_minimum_required(VERSION 3.16)

project(Server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party)

add_executable(Server main.cpp
                      udp_sender.cpp
                      ../third_party/md5/md5.cpp)

target_link_libraries(Server PRIVATE asio)