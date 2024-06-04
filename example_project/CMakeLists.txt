cmake_minimum_required(VERSION 3.20)

include(lib/cmake/gcc-arm-none-eabi.cmake)

project(example_project CXX C)

add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/main.cpp
    lib/syscalls.c
    lib/sysmem.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    lib
    src
)

target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/graphics_lib.a)