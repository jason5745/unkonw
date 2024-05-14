project(core)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME}_SOURCES)
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

set(${PROJECT_NAME}_SUB_PROJECT_NAMES)
file(GLOB FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach (FILE ${FILES})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}/CMakeLists.txt)
            add_subdirectory(${FILE})
            get_directory_property(SUB_PROJECT_NAME DIRECTORY ${FILE} DEFINITION PROJECT_NAME)
            list(APPEND ${PROJECT_NAME}_SUB_PROJECT_NAMES ${SUB_PROJECT_NAME})
        endif ()
    endif ()
endforeach ()
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(${PROJECT_NAME}
        PUBLIC ${${PROJECT_NAME}_SUB_PROJECT_NAMES})