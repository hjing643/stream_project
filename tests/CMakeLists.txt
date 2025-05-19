#find_package(GTest REQUIRED)       # 如果用系统安装的 gtest

#file(GLOB TEST_SOURCES "*.cpp")


# 定义测试可执行文件
#add_executable(test_suite ${TEST_SOURCES})

# header folder
#target_include_directories(test_suite BEFORE PRIVATE 
#    ${CMAKE_SOURCE_DIR}/include
#    ${MY_HOME_DIR}/third-party/ffmpeg6.0/linux_build/include
#)

# 链接测试框架和被测库
#target_link_libraries(test_suite
#    PRIVATE
#      GTest::gtest_main   # 如果用 gtest_main
#)

# 自动发现并注册测试
#include(GoogleTest)
#gtest_discover_tests(test_suite)
