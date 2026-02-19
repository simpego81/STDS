cmake_minimum_required(VERSION 3.10)
project(STDS_Tests)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GTest
find_package(GTest REQUIRED)

# Include directories
include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/../core/include
)

# Link directories
link_directories(
    ${PROJECT_SOURCE_DIR}/../core/lib
)

# Test executable
add_executable(test_core test_core.cpp)

# Link libraries
target_link_libraries(test_core
    ${GTEST_LIBRARIES}
    stds_core
    pthread
)

# Enable testing
enable_testing()
add_test(NAME CoreTests COMMAND test_core)
