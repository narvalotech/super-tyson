cmake_minimum_required(VERSION 3.19)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_FOUND})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API PREFER_SOURCE)

project(SuperTyson VERSION 1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/sexp.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_precompile_headers(${PROJECT_NAME} PRIVATE "<iostream>")
