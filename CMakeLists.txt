project(2048 C)

set( 2048_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( 2048_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )


add_executable( ${PROJECT_NAME} ${2048_SOURCES} )

add_subdirectory(src)

target_link_libraries( ${PROJECT_NAME} ${LIBS} raylib )


