cmake_minimum_required(VERSION 3.0.0)
project(youdis VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.83.0 REQUIRED)

add_executable(youdis main.cpp)
target_include_directories(youdis PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(youdis PRIVATE include)
