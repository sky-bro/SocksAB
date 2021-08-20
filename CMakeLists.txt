cmake_minimum_required(VERSION 3.5)

project(SocksAB
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(USE_DEPLOYQT OFF)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Network Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network Widgets REQUIRED)
find_package(Botan REQUIRED)

add_subdirectory(lib)
add_subdirectory(Socks-Alice)
add_subdirectory(Socks-Bob)

option(BUILD_TESTS "build tests or not" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)
