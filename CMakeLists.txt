cmake_minimum_required(VERSION 3.10)

project(minik-script VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES
	"src/*.cpp"
	"tests/*.cpp"
	"packages/*cpp"
)

include_directories("src" "tests")

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(raylib REQUIRED)
target_link_libraries(${PROJECT_NAME} raylib)
