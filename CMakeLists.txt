cmake_minimum_required(VERSION 3.22)
project(Mikroton VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin target
juce_add_plugin(Mikroton
    COMPANY_NAME "Mikroton"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Mktn
    PLUGIN_CODE Mktn
    FORMATS Standalone VST3
    PRODUCT_NAME "Mikroton"
    BUNDLE_ID "com.mikroton.mikroton"
)

# Source files
target_sources(Mikroton PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Synth/SynthVoice.cpp
    Source/Synth/SynthSound.cpp
    Source/Tuning/TuningSystem.cpp
    Source/Tuning/EqualTemperament.cpp
    Source/Tuning/JustIntonation.cpp
    Source/Tuning/Pythagorean.cpp
    Source/UI/KeyboardComponent.cpp
    Source/UI/TuningSelector.cpp
)

target_include_directories(Mikroton PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_compile_definitions(Mikroton PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Mikroton
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
