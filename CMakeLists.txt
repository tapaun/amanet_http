cmake_minimum_required(VERSION 3.20)
project(amanet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(amanet main.cpp
        src/amanet_client.cpp
        src/amanet_client.h
)

target_link_libraries(amanet PRIVATE Boost::system Boost::thread)