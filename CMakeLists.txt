cmake_minimum_required(VERSION 3.24)
project(SO_2022_ISEC C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(SO_2022_ISEC
        backend.c
        backend.h
        communication.h
        frontend.c
        frontend.h
        support_structures.h
        users_lib.h backend.c backend.c backend.c)
