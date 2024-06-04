cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Set project build settings
include(cmake/build-settings.cmake)

# Set the project name
set(CMAKE_PROJECT_NAME SoftwareOntwikkelingGroep8)
set(CMAKE_LIB_NAME ${CMAKE_PROJECT_NAME}Lib)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable CMake support for C++, C and ASM languages
enable_language(CXX C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Create a library with which can be linked
add_library(${CMAKE_LIB_NAME} STATIC)

target_compile_definitions(${CMAKE_LIB_NAME} PRIVATE 
    USE_HAL_DRIVER
    STM32F407xx
    $<$<CONFIG:Debug>:DEBUG>
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/business_logic/business_logic.cpp
    ${CMAKE_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_SOURCE_DIR}/Core/Src/CHAL.cpp
    ${CMAKE_SOURCE_DIR}/Core/Src/ASM_CHAL.s
)

target_sources(${CMAKE_LIB_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/API/text/API_gfx_text.cpp
    ${CMAKE_SOURCE_DIR}/API/system/Startup/startup_stm32f407vgtx.s
    ${CMAKE_SOURCE_DIR}/API/system/misc.c
    ${CMAKE_SOURCE_DIR}/API/system/stm32_ub_vga_screen.c
    ${CMAKE_SOURCE_DIR}/API/system/stm32f4xx_dma.c
    ${CMAKE_SOURCE_DIR}/API/system/stm32f4xx_gpio.c
    ${CMAKE_SOURCE_DIR}/API/system/stm32f4xx_rcc.c
    ${CMAKE_SOURCE_DIR}/API/system/stm32f4xx_tim.c
    ${CMAKE_SOURCE_DIR}/API/system/system_stm32f4xx.c
)

target_sources(${CMAKE_LIB_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/API/system/syscalls.c
    ${CMAKE_SOURCE_DIR}/API/system/sysmem.c
    ${CMAKE_SOURCE_DIR}/API/API_graphics.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/API/system
    ${CMAKE_SOURCE_DIR}/API
)

target_include_directories(${CMAKE_LIB_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/API/fonts
    ${CMAKE_SOURCE_DIR}/API/system
    ${CMAKE_SOURCE_DIR}/API/text
    ${CMAKE_SOURCE_DIR}/API/bitmaps
    ${CMAKE_SOURCE_DIR}/API
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_LIB_NAME}
)


# If clang-tidy is installed make a custom target,
# which can be run as a custom target or using all
find_program(CPPCHECK_EXE NAMES "cppcheck")
if (CPPCHECK_EXE)
    message("System name: " ${CMAKE_HOST_SYSTEM_NAME})

    get_target_property(SOURCES_LIST ${CMAKE_PROJECT_NAME} SOURCES)
    get_target_property(LIB_SOURCES_LIST ${CMAKE_LIB_NAME} SOURCES)
    list(APPEND SOURCES_LIST LIB_SOURCES_LIST)

    list(FILTER SOURCES_LIST INCLUDE REGEX "^.\.cpp|\.c$")

    message("Sources: " ${SOURCES_LIST})

    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        add_custom_target("cppcheck" COMMAND "${CMAKE_SOURCE_DIR}/cppcheck.bat" ${SOURCES_LIST}
        DEPENDS ${SOURCES_LIST} COMMENT "Analyzing ${CMAKE_PROJECT_NAME}" VERBATIM )
    endif()
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        add_custom_target("cppcheck" COMMAND "${CMAKE_SOURCE_DIR}/cppcheck.sh" ${SOURCES_LIST}
        DEPENDS ${SOURCES_LIST} COMMENT "Analyzing ${CMAKE_PROJECT_NAME}" VERBATIM )
    endif()
endif()
