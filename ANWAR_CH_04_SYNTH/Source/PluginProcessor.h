#pragma once
#include <JuceHeader.h>

class AnwarCH04AudioProcessor : public juce::AudioProcessor
{
public:
    AnwarCH04AudioProcessor();
    ~AnwarCH04AudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "ANWAR CH 04 SYNTH"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }
    int getNumPrograms() override { return 6; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    void setInstrument(int index);
    int getInstrument() const { return instrument.load(); }

private:
    struct Voice {
        double phase = 0.0;
        double frequency = 440.0;
        float velocity = 0.0f;
        float envelope = 0.0f;
        bool active = false;
        bool releasing = false;
        int note = -1;
    };
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void noteOn(int note, float velocity);
    void noteOff(int note);
    float nextSample(Voice& voice);

    std::array<Voice, 32> voices;
    std::atomic<int> instrument { 0 };
    int currentProgram = 0;
    double sr = 44100.0;
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnwarCH04AudioProcessor)
};
