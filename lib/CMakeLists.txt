set(SOURCE
    address.h
    address.cpp
    cipher.h
    cipher.cpp
    httpproxy.h
    httpproxy.cpp
    tcprelay.h
    tcprelay.cpp
    tcprelayclient.h
    tcprelayclient.cpp
    tcprelayserver.h
    tcprelayserver.cpp
    tcpserver.h
    tcpserver.cpp
    udprelay.h
    udprelay.cpp
    log.h
    log.cpp)

add_library(socksAB ${SOURCE})

target_include_directories(socksAB PUBLIC ${Botan_INCLUDE_DIR})

target_link_libraries(
  socksAB
  PUBLIC Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Botan::Botan)
