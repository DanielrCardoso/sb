cmake_minimum_required(VERSION 3.0)

project(Montador)

add_library(InstructionsUtils src/InstructionsUtils.cpp)
add_library(StringFileUtils src/StringFileUtils.cpp)

add_executable(montador montador.cpp)

target_link_libraries(montador InstructionsUtils StringFileUtils)

target_include_directories(InstructionsUtils PUBLIC include)
target_include_directories(StringFileUtils PUBLIC include)

target_include_directories(montador PUBLIC include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
