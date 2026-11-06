cmake_minimum_required(VERSION 3.22)
project(ANWARCH04SYNTH VERSION 1.0.0)

# Download JUCE and set JUCE_DIR to its CMake directory, or add JUCE as a subdirectory.
find_package(JUCE CONFIG QUIET)
if(NOT JUCE_FOUND)
    message(FATAL_ERROR "JUCE not found. Install JUCE 8 and set JUCE_DIR to its cmake folder.")
endif()

juce_add_plugin(ANWARCH04SYNTH
    COMPANY_NAME "ANWAR Audio"
    BUNDLE_ID "com.anwaraudio.ch04synth"
    PLUGIN_MANUFACTURER_CODE Anwr
    PLUGIN_CODE Aw04
    FORMATS VST3 Standalone
    PRODUCT_NAME "ANWAR CH 04 SYNTH"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(ANWARCH04SYNTH)

target_sources(ANWARCH04SYNTH PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(ANWARCH04SYNTH PRIVATE cxx_std_17)
target_compile_definitions(ANWARCH04SYNTH PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(ANWARCH04SYNTH PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
