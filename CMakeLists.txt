cmake_minimum_required(VERSION 4.1)
project(flat_shader LANGUAGES CXX)

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(WARNING "Consider configuring with: cmake -G Ninja ..")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(CMAKE_LINKER "ld" CACHE STRING "Linker to use")

set(CMAKE_CONFIGURE_DEPENDS ON)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME}
    PRIVATE
        "src/pch.hpp"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        m
        SDL3
        GL
        GLU
        GLEW
        assimp
)
