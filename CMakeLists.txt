cmake_minimum_required(VERSION 3.14)

project(MyProject VERSION 1.0 DESCRIPTION "Anti-cheat testing tool" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(actt ${SOURCES})
target_include_directories(actt PRIVATE "src/")
