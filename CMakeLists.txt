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


list(REMOVE_ITEM WINCHESS_FILES "ann/*.*")
list(REMOVE_ITEM WINCHESS_TEST_FILES "ann/*.*")


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ann/model/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ann/model/)

add_executable(winchess ${WINCHESS_FILES})
add_executable(winchess_tests ${WINCHESS_TEST_FILES})

if(WIN32)
  target_link_libraries(winchess ${CMAKE_CURRENT_SOURCE_DIR}/ann/tensorflow-c-library/windows/tensorflow.lib)
  target_link_libraries(winchess_tests ${CMAKE_CURRENT_SOURCE_DIR}/ann/tensorflow-c-library/windows/tensorflow.lib)
endif()

if (UNIX)
	target_link_libraries(winchess "atomic")
	target_link_libraries(winchess_tests "atomic")

	target_link_libraries(winchess "pthread")
	target_link_libraries(winchess_tests "pthread")

    #probably does not work
    target_link_libraries(winchess ${CMAKE_CURRENT_SOURCE_DIR}/ann/tensorflow-c-library/linux/libtensorflow.so.1.15)
    target_link_libraries(winchess_tests ${CMAKE_CURRENT_SOURCE_DIR}/ann/tensorflow-c-library/linux/libtensorflow.so.1.15)


endif (UNIX)