cmake_minimum_required(VERSION 3.5)

project(SocksAB
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Network Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network Widgets LinguistTools REQUIRED)

find_package(PkgConfig)
pkg_search_module(BOTAN REQUIRED botan-2>=2.3.0)
find_library(BOTAN_LIBRARY_VAR
    NAMES ${BOTAN_LIBRARIES}
    HINTS ${BOTAN_LIBRARY_DIRS} ${BOTAN_LIBDIR})

#message(Qt${QT_VERSION_MAJOR}) # Qt5
add_subdirectory(lib)
add_subdirectory(Socks-Alice)
add_subdirectory(Socks-Bob)
