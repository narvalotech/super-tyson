cmake_minimum_required(VERSION 3.19)

project(st VERSION 1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME} SuperTyson.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE target)
target_link_libraries(${PROJECT_NAME} PRIVATE be tracker translation)

target_precompile_headers(${PROJECT_NAME} PRIVATE "<iostream>" "<sstream>" "<string>" "<string_view>")
