cmake_minimum_required(VERSION 3.29)
project(Talento)

set(CMAKE_CXX_STANDARD 20)

add_executable(Talento main.cpp
        src/bitboard.hpp
        src/utils.hpp
        src/magic.hpp
        src/movegen.hpp
        src/movelist.hpp)
