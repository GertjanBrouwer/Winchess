cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(Winchess VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB WINCHESS_FILES
    "src/*.h"
    "src/*.cpp"
    "Winchess.cpp"
)

file(GLOB WINCHESS_TEST_FILES
    "src/*.h"
    "src/*.cpp"
    "tests/*.h"
    "tests/*.cpp"
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

add_executable(winchess ${WINCHESS_FILES})
add_executable(winchess_tests ${WINCHESS_TEST_FILES})

if (UNIX)
	target_link_libraries(winchess "atomic")
	target_link_libraries(winchess_tests "atomic")

	target_link_libraries(winchess "pthread")
	target_link_libraries(winchess_tests "pthread")

endif (UNIX)