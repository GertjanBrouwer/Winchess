cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(Winchess VERSION 1.0)
set(CMAKE_CXX_STANDARD 14)


file(GLOB WINCHESS_FILES
    "src/*.h"
    "src/*.cpp"
)

file(GLOB WINCHESS_TEST_FILES
    "src/*.h"
    "src/*.cpp"
    "tests/*.h"
    "tests/*.cpp"
)

list(REMOVE_ITEM winchess_tests "src/Main.cpp")

add_executable(winchess ${WINCHESS_FILES})
add_executable(winchess_tests ${WINCHESS_TEST_FILES})