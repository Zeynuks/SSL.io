cmake_minimum_required(VERSION 3.10)

project(SSL.io)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SFML REQUIRED COMPONENTS system window graphics audio network)

add_executable(SSL.io main.cpp)

target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-audio sfml-network)