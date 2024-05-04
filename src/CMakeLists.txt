project(main)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} main_source)
add_executable(${CMAKE_OUT_PUT} ${main_source})


add_subdirectory(core)
add_subdirectory(utils)