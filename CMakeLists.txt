cmake_minimum_required(VERSION 3.12)
project(MetaWave_QC_Compiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
endif()

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create the library
add_library(qc_expression_tree STATIC ${SOURCES} ${HEADERS})
target_include_directories(qc_expression_tree PUBLIC include)

# Example executable
add_executable(qc_example examples/main.cpp)
target_link_libraries(qc_example qc_expression_tree)

# Tests
enable_testing()
add_subdirectory(tests)

# Installation
install(TARGETS qc_expression_tree DESTINATION lib)
install(DIRECTORY include/ DESTINATION include) 