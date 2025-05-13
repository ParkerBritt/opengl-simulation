cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(simulation)

add_executable(${PROJECT_NAME}
    src/main.cpp
    extern/glad/src/glad.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
    extern/glad/include
)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

# glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
