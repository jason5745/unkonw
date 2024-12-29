project(application)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME}_SOURCES)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

include_directories(${CMAKE_BINARY_DIR}/src/model)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(model)
add_subdirectory(controller)
add_subdirectory(service)

target_link_libraries(application
    PRIVATE
        model
        controller
        service
)

target_link_libraries(application
    PRIVATE
        ${PROTOBUF_LIBRARIES}
        ${Boost_LIBRARIES}
        ${EXTERNAL_LIBRARIES}
)