cmake_minimum_required(VERSION 3.16)

project(Server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Server main.cpp
                      udp_sender.cpp
                      ../third_party/md5/md5.cpp)

if (UNIX) 
    target_link_libraries(Server PRIVATE pthread)  # boost_system boost_thread
endif()
target_link_libraries(Server PRIVATE asio)