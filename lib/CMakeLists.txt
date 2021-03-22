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
    )

add_library(socksAB ${SOURCE})

target_link_libraries(socksAB
    PUBLIC Qt${QT_VERSION_MAJOR}::Network
    PRIVATE ${BOTAN_LIBRARY_VAR})

target_include_directories(socksAB
    PRIVATE ${BOTAN_INCLUDE_DIRS})

message(${BOTAN_INCLUDE_DIRS})
