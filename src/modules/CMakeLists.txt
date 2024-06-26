project(modules)

set(MODULES)
file(GLOB FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach (FILE ${FILES})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
        aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${FILE} ${FILE}_SOURCES)
        add_library(${FILE} ${${FILE}_SOURCES})
        target_compile_options(${FILE} PUBLIC -Wall)
        target_include_directories(${FILE} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${FILE})
        target_link_libraries(${FILE} PUBLIC utils)
        list(APPEND MODULES ${FILE})
    endif ()
endforeach ()

add_library(${PROJECT_NAME} STATIC module_factory.cpp)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULES})
