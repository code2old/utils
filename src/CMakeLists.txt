aux_source_directory(. SRC_LIST)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(utils_static STATIC ${SRC_LIST})
add_library(utils_shared SHARED ${SRC_LIST})

set_target_properties(utils_static PROPERTIES OUTPUT_NAME "utils" )
set_target_properties(utils_shared PROPERTIES OUTPUT_NAME "utils" VERSION 1.2 SOVERSION 1)

INSTALL(
    TARGETS  utils_shared utils_static
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)
INSTALL(FILES 
    ${PROJECT_SOURCE_DIR}/include/thread.h 
    ${PROJECT_SOURCE_DIR}/include/runnable.h 
    DESTINATION include)