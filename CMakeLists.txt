cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(simulation)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
