project(application)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME}_SOURCES)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

include_directories(${CMAKE_BINARY_DIR}/src/dto)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(dto)
add_subdirectory(router)
add_subdirectory(controller)

target_link_libraries(application
        PRIVATE
            dto
            router
            controller
            ${PROTOBUF_LIBRARIES}
            ${Boost_LIBRARIES}
)
