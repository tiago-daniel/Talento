cmake_minimum_required(VERSION 3.29)
project(Talento)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-O3")

set(CMAKE_CXX_STANDARD 20)

add_executable(Talento main.cpp
        src/bitboard.hpp
        src/utils.hpp
        src/magic.hpp
        src/movegen.hpp
        src/movelist.hpp
        src/board.hpp
)
