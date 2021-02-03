add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
add_executable(memory_test memory_test.cpp)
target_link_libraries(memory_test gtest_main)
add_test(memory_test_exe memory_test)
enable_testing()