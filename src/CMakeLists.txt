project(main)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCES)

add_subdirectory(utils)

add_executable(main ${SOURCES})

target_link_libraries(main utils)

