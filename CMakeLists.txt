cmake_minimum_required(VERSION 3.21)
project(RhythmRealms)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find SFML 3.0
find_package(SFML 3.0 COMPONENTS Graphics System Audio Window REQUIRED)

# Add Candle as subdirectory
add_subdirectory(Candle)

# Collect all source files from Capstone Project folder
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Capstone Project/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Capstone Project/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Capstone Project/*.h"
)

# Create executable
add_executable(RhythmRealms ${SOURCES})

# Include directories
target_include_directories(RhythmRealms PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Capstone Project"
    "${CMAKE_CURRENT_SOURCE_DIR}/Candle/include"
)

# Link libraries
target_link_libraries(RhythmRealms PRIVATE
    SFML::Graphics
    SFML::System
    SFML::Audio
    SFML::Window
    Candle-s
)

# Set subsystem for Windows (removes console window in Release)
if(WIN32)
    set_target_properties(RhythmRealms PROPERTIES
        WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>
    )
endif()

# Copy SFML DLLs to output directory (Windows)
if(WIN32)
    add_custom_command(TARGET RhythmRealms POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SFML::Graphics>
            $<TARGET_FILE:SFML::System>
            $<TARGET_FILE:SFML::Audio>
            $<TARGET_FILE:SFML::Window>
            $<TARGET_FILE_DIR:RhythmRealms>
    )
endif()