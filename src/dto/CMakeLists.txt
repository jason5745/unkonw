project(dto)

file(GLOB PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
set(PROTO_SRCS "")
set(PROTO_HDRS "")

foreach(PROTOBUF_FILE ${PROTO_FILES})
    protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTOBUF_FILE})
    list(APPEND PROTO_SRCS ${PROTO_SRC})
    list(APPEND PROTO_HDRS ${PROTO_HDR})
endforeach()

add_library(dto OBJECT ${PROTO_SRCS})
target_link_libraries(${CMAKE_OUT_PUT} dto)