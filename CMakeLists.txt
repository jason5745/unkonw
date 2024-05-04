
cmake_minimum_required(VERSION 3.16)  # 假设项目需要至少CMake 3.16
project(app)     # 假设项目使用C++，若为C则改为"C"


set(CMAKE_CXX_STANDARD 23)           # 设置C++标准（根据实际项目需求调整）
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 确保编译器支持指定的C++标准
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_OUT_PUT "main")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

find_package(protobuf CONFIG REQUIRED)
include(FindProtobuf)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--copy-dt-needed-entries")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--copy-dt-needed-entries")

find_package(Boost COMPONENTS coroutine context url REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(include)
include_directories(include/element)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/spdlog/include)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)

target_link_libraries(${CMAKE_OUT_PUT} ${Boost_LIBRARIES})
target_link_libraries(${CMAKE_OUT_PUT} ${PROTOBUF_LIBRARIES})


# 6. 安装规则
# install(TARGETS lib DESTINATION lib)
# install(TARGETS app DESTINATION bin)
# install(DIRECTORY include/ DESTINATION include)

# 7. 测试与文档生成（可选）
# enable_testing()                       # 启用CTest支持
# find_package(GTest REQUIRED)          # 引入Google Test
# if(GTEST_FOUND)
#     include_directories(${GTEST_INCLUDE_DIRS})
#     add_subdirectory(tests)            # 包含测试子目录（如果有）
# endif()

# find_package(Doxygen)                 # 查找Doxygen
# if(DOXYGEN_FOUND)
#     configure_file(Doxyfile.in Doxyfile @ONLY)
#     add_custom_target(doc
#         COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#         COMMENT "Generating API documentation with Doxygen"
#         VERBATIM)
# endif()