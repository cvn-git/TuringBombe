cmake_minimum_required(VERSION 3.22)

project(TuringBombe VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
