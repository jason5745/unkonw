project(service)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME}_SOURCES)
add_library(${PROJECT_NAME} OBJECT ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE: ${CMAKE_CURRENT_SOURCE_DIR}>)
