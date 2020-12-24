cmake_minimum_required(VERSION 3.5)

project(SocksAB
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lib)
add_subdirectory(Socks-Alice)
add_subdirectory(Socks-Bob)
