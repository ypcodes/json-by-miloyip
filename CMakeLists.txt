cmake_minimum_required(VERSION 3.0)

project(leptjson_test)

# 添加 C++11 标准支持
set(CMAKE_CXX_STANDARD 17)

# # 添加 GoogleTest 子目录
# add_subdirectory(googletest)

# # 包含 GoogleTest 头文件目录
# include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 编译 leptjson 库
add_library(leptjson leptjson.cpp)

# 编译 leptjson_test 可执行文件
add_executable(leptjson_test test.cpp)

# 链接 leptjson 库和 GoogleTest 库
target_link_libraries(leptjson_test leptjson gtest gtest_main)

# 设置编译类型
set(CMAKE_BUILD_TYPE Debug)

# 设置编译选项
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")
