project(utils)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} utils_sources)
add_library(utils OBJECT ${utils_sources})

target_link_libraries(${CMAKE_OUT_PUT} utils)