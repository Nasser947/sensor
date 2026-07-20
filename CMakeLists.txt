cmake_minimum_required(VERSION 3.10)
project(sensor C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/*.c")

add_executable(sensor ${SOURCES})

target_include_directories(sensor PRIVATE ${CMAKE_SOURCE_DIR})
