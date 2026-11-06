#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AnwarCH04AudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    explicit AnwarCH04AudioProcessorEditor(AnwarCH04AudioProcessor&);
    ~AnwarCH04AudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    void styleButton(juce::TextButton&);
    AnwarCH04AudioProcessor& processor;
    juce::ComboBox instruments;
    juce::Slider master, attack, release, reverb;
    juce::Label title, subtitle, meterLabel;
    juce::TextButton presetButton { "LOAD PRESET" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterAttach, attackAttach, releaseAttach, reverbAttach;
    float meterPhase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnwarCH04AudioProcessorEditor)
};
