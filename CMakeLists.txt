
cmake_minimum_required(VERSION 3.16)  # 假设项目需要至少CMake 3.16
project(GatewayApplication)     # 假设项目使用C++，若为C则改为"C"

set(CMAKE_CXX_STANDARD 23)           # 设置C++标准（根据实际项目需求调整）
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 确保编译器支持指定的C++标准
#set(CMAKE_BUILD_TYPE Debug)

find_package(protobuf CONFIG REQUIRED)
include(FindProtobuf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--copy-dt-needed-entries")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--copy-dt-needed-entries")

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/spdlog/include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)

