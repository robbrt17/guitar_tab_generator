cmake_minimum_required(VERSION 3.22)

# 1. Fixed the Project Name
project(AdaptiveDistortion VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. JUCE Library fetch (Your method - excellent choice)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)
FetchContent_MakeAvailable(JUCE)

# 3. Changed to juce_add_plugin
juce_add_plugin(AdaptiveDistortion
    FORMATS Standalone VST3               # Builds a standalone testing app AND a DAW plugin
    PLUGIN_MANUFACTURER_CODE "Ths1"       # Must be exactly 4 characters
    PLUGIN_CODE "Adpt"                    # Must be exactly 4 characters
    COMPANY_NAME "YourName"
    PLUGIN_NAME "Adaptive Distortion"
    IS_SYNTH FALSE                        # It's an effect, not an instrument
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE          # Puts the VST3 in your system folders automatically
)

# 4. Pointed to the correct audio plugin source files
target_sources(AdaptiveDistortion PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
)

# 5. Necessary JUCE modules
target_link_libraries(AdaptiveDistortion PRIVATE
    juce::juce_audio_utils   # Required for the standalone app wrapper
    juce::juce_dsp           # Required for your distortion math
    juce::juce_gui_basics
    juce::juce_audio_formats
)

target_compile_definitions(AdaptiveDistortion PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0)

juce_generate_juce_header(AdaptiveDistortion)