set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE
    mainDialog.cpp
    mainDialog.h
    mainDialog.ui
    loginDialog.cpp
    loginDialog.h
    loginDialog.ui
    serverEditDialog.cpp
    serverEditDialog.h
    serverEditDialog.ui
    main.cpp)

if(COMMAND qt_add_resources)
  qt_add_resources(SOURCE res.qrc)
else()
  qt5_add_resources(SOURCE res.qrc)
endif()

if(WIN32)
  add_executable(Socks-Alice WIN32 ${SOURCE})
else()
  add_executable(Socks-Alice ${SOURCE})
endif()

target_link_libraries(Socks-Alice PRIVATE socksAB)
target_link_libraries(Socks-Alice PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(
  Socks-Alice
  PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/lib"
  PRIVATE ${BOTAN_INCLUDE_DIRS})

if(UNIX
   AND NOT APPLE
   AND NOT WIN32)
  install(TARGETS Socks-Alice RUNTIME DESTINATION bin)
  install(FILES Socks-Alice.desktop DESTINATION share/applications)
  install(
    FILES SocksAB.png
    DESTINATION share/icons
    RENAME SocksAB.png)
endif()

if(WIN32)
  install(TARGETS Socks-Alice RUNTIME DESTINATION alice)
  if(USE_DEPLOYQT)
    add_custom_command(
      TARGET Socks-Alice
      POST_BUILD
      COMMAND
        ${Qt5_DIR}/../../../bin/windeployqt
        ${CMAKE_CURRENT_BINARY_DIR}/Socks-Alice.exe --compiler-runtime --verbose
        2 --dir ${CMAKE_CURRENT_BINARY_DIR}/winqt-alice/
      COMMAND ${CMAKE_COMMAND} -E copy ${Botan_LIBRARY}/../../bin/botan.dll
              ${CMAKE_CURRENT_BINARY_DIR}/winqt-alice/)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/winqt-alice/
            DESTINATION alice)
  endif()
endif()

if(APPLE)
  set(MACOSX_ICON "${CMAKE_SOURCE_DIR}/assets/icons/SocksAB.icns")
  # set(MACOSX_PLIST "MacOSXBundleInfo.plist.in")
  # set_source_files_properties(${QM_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION
  # Resources/lang )

  target_sources(Socks-Alice PRIVATE ${MACOSX_ICON})

  set_target_properties(
    Socks-Alice
    PROPERTIES MACOSX_BUNDLE TRUE
               # MACOSX_BUNDLE_INFO_PLIST ${MACOSX_PLIST}
               MACOSX_BUNDLE_BUNDLE_NAME "Socks-Alice"
               # MACOSX_BUNDLE_BUNDLE_VERSION ${QV2RAY_VERSION_STRING}
               # MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2019-2020 Qv2ray
               # Development Group"
               # MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.qv2ray"
               # MACOSX_BUNDLE_ICON_FILE "qv2ray.icns"
               # MACOSX_BUNDLE_INFO_STRING "Created by Qv2ray development team"
               # MACOSX_BUNDLE_LONG_VERSION_STRING ${QV2RAY_VERSION_STRING}
               # MACOSX_BUNDLE_SHORT_VERSION_STRING ${QV2RAY_VERSION_STRING}
               RESOURCE ${MACOSX_ICON})

  # Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}

  install(
    TARGETS Socks-Alice
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime)
  if(USE_DEPLOYQT)
    add_custom_command(
      TARGET Socks-Alice
      POST_BUILD
      COMMAND ${Qt5_DIR}/../../../bin/macdeployqt
              ${CMAKE_CURRENT_BINARY_DIR}/Socks-Alice.app)
  endif()
  set(APPNAME "Socks-Alice")
  configure_file("${CMAKE_SOURCE_DIR}/assets/package_dmg.json.in"
                 "${CMAKE_CURRENT_SOURCE_DIR}/assets/package_dmg.json" @ONLY)
endif()
