cmake_minimum_required(VERSION 3.18.1)

project("workouttracker")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile flags
add_compile_options(-Wall -Wextra -Werror)

# Find required packages
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv2-lib GLESv2)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp)

# Source files
set(SOURCES
    src/main/cpp/main.cpp
    src/main/cpp/App.cpp
    src/main/cpp/Renderer.cpp
    src/main/cpp/InputHandler.cpp
    src/main/cpp/WorkoutTracker.cpp
    src/main/cpp/TextRenderer.cpp
    src/main/cpp/Button.cpp
    src/main/cpp/IconRenderer.cpp
    src/main/cpp/android_native_app_glue.c
)

# Create shared library
add_library(workouttracker SHARED ${SOURCES})

# Link libraries
target_link_libraries(workouttracker
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv2-lib}
)

