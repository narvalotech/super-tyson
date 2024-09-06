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

set(CMAKE_BUILD_TYPE Debug)

add_library(target STATIC
  src/target.cpp
  src/serial/linux.cpp
  src/sexp.cpp)

set_target_properties(target PROPERTIES PUBLIC_HEADER
  ${CMAKE_CURRENT_SOURCE_DIR}/include/st/target.h)

target_include_directories(target PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

project(SuperTyson VERSION 1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE target)

target_precompile_headers(${PROJECT_NAME} PRIVATE "<iostream>")
