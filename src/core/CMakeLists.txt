project(core)

add_subdirectory(dto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} core_sources)
add_library(core OBJECT ${core_sources})

target_link_libraries(${CMAKE_OUT_PUT} core)
