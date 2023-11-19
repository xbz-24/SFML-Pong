cmake_minimum_required(VERSION 3.0)
project(Pong)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(Box2D CONFIG REQUIRED)

add_executable(pong main.cpp)

target_link_libraries(pong sfml-graphics sfml-window sfml-system Box2D)
