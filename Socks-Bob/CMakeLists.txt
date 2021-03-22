add_executable(Socks-Bob main.cpp)

target_link_libraries(Socks-Bob socksAB)

target_include_directories(Socks-Bob PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/lib"
                          PRIVATE ${BOTAN_INCLUDE_DIRS}
                          )
