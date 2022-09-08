cmake_minimum_required(VERSION 3.11)

project(Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(Client main.cpp
                      udp_receiver.cpp
                      processing.cpp
                      ../third_party/md5/md5.cpp)

if (UNIX) 
    target_link_libraries(Client PRIVATE pthread) 
endif()
target_link_libraries(Client PRIVATE asio)