
cmake_minimum_required(VERSION 3.16)  # 假设项目需要至少CMake 3.16
project(app)     # 假设项目使用C++，若为C则改为"C"


set(CMAKE_CXX_STANDARD 23)           # 设置C++标准（根据实际项目需求调整）
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 确保编译器支持指定的C++标准
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

# 假设源文件位于src目录，头文件位于include目录
include_directories(include)

#Boost
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../boost/include)
set(BOOST_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../boost/lib")
file(GLOB BOOST_LIBRARY_FILES "${BOOST_LIBRARY_DIR}/*.so" "${BOOST_LIBRARY_DIR}/*.a")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})


add_subdirectory(src)


target_link_libraries(main ${BOOST_LIBRARY_FILES})


# 5. 依赖管理
# 使用find_package查找并链接外部库（如Boost、OpenCV等）
# 示例：
# find_package(Boost REQUIRED COMPONENTS log system)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     target_link_libraries(main ${Boost_LIBRARIES})
# endif()

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