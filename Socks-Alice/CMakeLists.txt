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

install(TARGETS Socks-Alice RUNTIME DESTINATION bin)
install(FILES Socks-Alice.desktop DESTINATION share/applications)
