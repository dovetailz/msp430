cmake_minimum_required(VERSION 3.10)

# Project name
project(MSP430 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(MSP430 msp430.cpp)

add_subdirectory(processor)
target_link_libraries(MSP430 PUBLIC processor)
target_include_directories(MSP430 PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            "${PROJECT_SOURCE_DIR}/processor")

add_subdirectory(memory)
target_link_libraries(MSP430 PUBLIC processor)
target_include_directories(MSP430 PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            "${PROJECT_SOURCE_DIR}/memory")

configure_file(msp430.h.in msp430.h)
configure_file(.clang-format .clang-format)
add_custom_target(format
    COMMAND ../format.sh)