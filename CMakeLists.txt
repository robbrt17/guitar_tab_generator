cmake_minimum_required(VERSION 3.22)

# Define your project name and language
project(GuitarTabGenerator VERSION 1.0.0 LANGUAGES CXX)

# Force CMake to use C++17 or C++20 (JUCE requires at least C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------
# Automatically Download JUCE via FetchContent
# -------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0 # Or specify a preferred stable version like 7.0.12
)
FetchContent_MakeAvailable(JUCE)

# -------------------------------------------------------------------
# Define Your Executable Target
# -------------------------------------------------------------------
# juce_add_gui_app creates a standalone desktop application
juce_add_gui_app(GuitarTabGenerator
    COMPANY_NAME "YourName"
    PRODUCT_NAME "Guitar Tab Generator"
)

# -------------------------------------------------------------------
# Include Source Files
# -------------------------------------------------------------------
target_sources(GuitarTabGenerator PRIVATE
    src/Main.cpp
    src/MainComponent.h
    src/MainComponent.cpp
)

# -------------------------------------------------------------------
# Link Necessary JUCE Modules
# -------------------------------------------------------------------
# For Phase 1, you need gui_basics and audio_formats
target_link_libraries(GuitarTabGenerator PRIVATE
    juce::juce_gui_basics
    juce::juce_audio_formats
    juce::juce_audio_utils
    juce::juce_dsp
)

# Enable standard features like format registration
juce_generate_juce_header(GuitarTabGenerator)