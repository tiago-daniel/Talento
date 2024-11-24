cmake_minimum_required(VERSION 3.29)
project(Talento)

#set(CMAKE_BUILD_TYPE Debug)

# Add global compiler options
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined,thread -g -Wall -Wextra")

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-O3")

# WINDOWS STATIC EXE

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
#set(CMAKE_SYSTEM_NAME Windows)
#set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
#set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")


set(CMAKE_CXX_STANDARD 20)

add_executable(Talento main.cpp
        src/bitboard.hpp
        src/utils.hpp
        src/magic.hpp
        src/movegen.hpp
        src/movelist.hpp
        src/board.hpp
        src/search.hpp
        src/evaluation.hpp
        src/uci.hpp
)
