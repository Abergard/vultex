cmake_minimum_required(VERSION 3.15.4)

project(vultex CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory(src)
