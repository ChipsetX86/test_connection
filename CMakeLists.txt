cmake_minimum_required(VERSION 3.11)

project(TestConnection LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(asio)
include(GoogleTest)

add_subdirectory(Client)
add_subdirectory(Server)