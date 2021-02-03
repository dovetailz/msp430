add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
include_directories(${CMAKE_SOURCE_DIR}/memory/include)
add_subdirectory(src)
enable_testing()