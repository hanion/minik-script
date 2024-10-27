cmake_minimum_required(VERSION 3.10)

project(minik-script VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES "src/*.cpp" "tests/*.cpp")

include_directories("src" "tests")

add_executable(${PROJECT_NAME} ${SOURCES})
