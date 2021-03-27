set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE
    dialog.cpp
    dialog.h
    dialog.ui
    logindialog.cpp
    logindialog.h
    logindialog.ui
    editdialog.cpp
    editdialog.h
    editdialog.ui
    main.cpp)

add_executable(Socks-Alice ${SOURCE})

target_link_libraries(Socks-Alice PRIVATE socksAB)
target_link_libraries(Socks-Alice PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(Socks-Alice
    PUBLIC "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/lib"
    PRIVATE ${BOTAN_INCLUDE_DIRS}
    )

if (UNIX AND NOT APPLE AND NOT WIN32)
    install(TARGETS Socks-Alice RUNTIME DESTINATION bin)
    install(FILES Socks-Alice.desktop DESTINATION share/applications)
    install(FILES heart.png DESTINATION share/icons RENAME SocksAB.png)
endif()

if(WIN32)
    install(TARGETS Socks-Alice RUNTIME DESTINATION alice)
    if(USE_WINDEPLOYQT)
        add_custom_command(TARGET Socks-Alice POST_BUILD
            COMMAND ${Qt5_DIR}/../../../bin/windeployqt ${CMAKE_BINARY_DIR}/Socks-Alice.exe --compiler-runtime --verbose 2
            )
    endif()
endif()
