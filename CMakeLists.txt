cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(Tutorial VERSION 1.0)
set(CMAKE_CXX_STANDARD 14)

# add the executable
add_executable(cmake_winchess src/Winchess.cpp)