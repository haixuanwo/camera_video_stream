cmake_minimum_required(VERSION 3.5)

project(camera_video_stream VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定头文件路径
include_directories(3rd_party/muduo/include)
include_directories(3rd_party/opencv/include)

# 链接库路径
link_directories(3rd_party/muduo/lib)
link_directories(3rd_party/opencv/lib/opencv)

add_subdirectory(client)
add_subdirectory(server)
