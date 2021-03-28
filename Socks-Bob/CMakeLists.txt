add_executable(Socks-Bob main.cpp)

target_link_libraries(Socks-Bob socksAB)

target_include_directories(Socks-Bob PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/lib"
                          PRIVATE ${BOTAN_INCLUDE_DIRS}
                          )
if (UNIX AND NOT APPLE AND NOT WIN32)
    install(TARGETS Socks-Bob RUNTIME DESTINATION bin)
    install(FILES Socks-Bob.desktop DESTINATION share/applications)
endif()

if(WIN32)
    install(TARGETS Socks-Bob RUNTIME DESTINATION bob)
    if(USE_WINDEPLOYQT)
        add_custom_command(TARGET Socks-Bob POST_BUILD
            COMMAND ${Qt5_DIR}/../../../bin/windeployqt ${CMAKE_CURRENT_BINARY_DIR}/Socks-Bob.exe --compiler-runtime --verbose 2 --dir ${CMAKE_CURRENT_BINARY_DIR}/winqt-bob/
            COMMAND ${CMAKE_COMMAND} -E copy ${Botan_LIBRARY}/../../bin/botan.dll ${CMAKE_CURRENT_BINARY_DIR}/winqt-bob/
            )
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/winqt-bob/ DESTINATION bob)
endif()
