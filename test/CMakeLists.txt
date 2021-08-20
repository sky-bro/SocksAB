find_package(GTest REQUIRED)
# GTest 的头文件
include_directories(${GTEST_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/lib")

add_executable(test_cipher lib/test_cipher.cpp)

# 链接测试库
target_link_libraries(test_cipher
       ${GTEST_BOTH_LIBRARIES}
       pthread socksAB )

# 添加到测试
gtest_discover_tests(test_cipher)
