project(main)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCES)
add_executable(main ${SOURCES})

add_subdirectory(utils)
target_link_libraries(main utils)

