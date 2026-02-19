cmake_minimum_required(VERSION 3.10)
project(STDS_Core VERSION 1.0.0 LANGUAGES CXX)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position-independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/Normalizer.cpp
    src/SequenceTree.cpp
    src/STDSEngine.cpp
)

# Create static library
add_library(stds_core STATIC ${SOURCES})

# Set output directory
set_target_properties(stds_core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

# Install targets
install(TARGETS stds_core
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
