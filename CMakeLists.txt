
cmake_minimum_required(VERSION 3.16)  # 假设项目需要至少CMake 3.16
project(GatewayApplication)     # 假设项目使用C++，若为C则改为"C"

set(CMAKE_CXX_STANDARD 23)           # 设置C++标准（根据实际项目需求调整）
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 确保编译器支持指定的C++标准
#set(CMAKE_BUILD_TYPE Debug)

set(SPDLOG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/spdlog/include")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "You are building on Linux")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--copy-dt-needed-entries")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--copy-dt-needed-entries")
    set(EXTERNAL_LIBRARIES)

elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "You are building on Windows")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(EXTERNAL_LIBRARIES)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "You are building on macOS")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(EXTERNAL_LIBRARIES absl::status absl::absl_log absl::absl_check)

else()
    message(STATUS "You are building on another system")

endif()

find_package(protobuf CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED)

include(FindProtobuf)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${SPDLOG_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)


#
#set(ABSEL_LIBRARIES
#        absl::absl_check
#        absl::absl_log
#        absl::algorithm
#        absl::base
#        absl::bind_front
#        absl::bits
#        absl::btree
#        absl::cleanup
#        absl::cord
#        absl::core_headers
#        absl::debugging
#        absl::die_if_null
#        absl::dynamic_annotations
#        absl::flags
#        absl::flat_hash_map
#        absl::flat_hash_set
#        absl::function_ref
#        absl::hash
#        absl::layout
#        absl::log_initialize
#        absl::log_severity
#        absl::memory
#        absl::node_hash_map
#        absl::node_hash_set
#        absl::optional
#        absl::span
#        absl::status
#        absl::statusor
#        absl::strings
#        absl::synchronization
#        absl::time
#        absl::type_traits
#        absl::utility
#        absl::variant
#)
