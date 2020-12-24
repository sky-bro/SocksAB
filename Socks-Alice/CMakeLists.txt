
add_executable(Socks-Alice main.cpp)

target_link_libraries(Socks-Alice socksAB)

target_include_directories(Socks-Alice PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/lib"
                          )
