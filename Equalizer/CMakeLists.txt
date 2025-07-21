cmake_minimum_required(VERSION 3.31)
project(Vocal_Enhancer VERSION 0.0.1)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(Equalizer STATIC
        src/Equalizer_VE.cpp
)

target_include_directories(Equalizer
        PUBLIC
        ${PROJECT_SOURCE_DIR}/Include
)

add_library(Equalizer_VE::Equalizer_VE ALIAS Equalizer)

target_link_libraries(Equalizer
        PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_basics
        juce::juce_core
)