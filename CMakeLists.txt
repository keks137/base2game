project(2048 C)

set( 2048_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( 2048_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable( ${PROJECT_NAME} ${2048_SOURCES} )

add_subdirectory(src)

target_link_libraries( ${PROJECT_NAME} ${LIBS} raylib )


