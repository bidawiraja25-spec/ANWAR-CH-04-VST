#include "PluginProcessor.h"
#include "PluginEditor.h"

AnwarCH04AudioProcessor::AnwarCH04AudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AnwarCH04AudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<AudioParameterFloat>("gain", "Master", NormalisableRange<float>(0.0f, 1.0f), 0.72f));
    p.add(std::make_unique<AudioParameterFloat>("attack", "Attack", NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.4f), 0.02f));
    p.add(std::make_unique<AudioParameterFloat>("release", "Release", NormalisableRange<float>(0.02f, 4.0f, 0.001f, 0.4f), 0.35f));
    p.add(std::make_unique<AudioParameterFloat>("reverb", "Reverb", NormalisableRange<float>(0.0f, 1.0f), 0.22f));
    p.add(std::make_unique<AudioParameterFloat>("tone", "Tone", NormalisableRange<float>(0.0f, 1.0f), 0.55f));
    return p;
}

void AnwarCH04AudioProcessor::prepareToPlay(double sampleRate, int)
{
    sr = sampleRate;
    reverbParams.roomSize = 0.55f;
    reverbParams.damping = 0.35f;
    reverbParams.wetLevel = 0.22f;
    reverbParams.dryLevel = 0.85f;
    reverb.setParameters(reverbParams);
    for (auto& v : voices) v = Voice{};
}
void AnwarCH04AudioProcessor::releaseResources() {}

void AnwarCH04AudioProcessor::setInstrument(int index)
{
    instrument.store(juce::jlimit(0, 5, index));
    currentProgram = instrument.load();
}
const juce::String AnwarCH04AudioProcessor::getProgramName(int index)
{
    static const char* names[] = { "Acoustic Piano", "Acoustic Guitar", "Electric Guitar", "Violin", "Synth", "Reel Keys" };
    return (index >= 0 && index < 6) ? names[index] : "";
}
void AnwarCH04AudioProcessor::setCurrentProgram(int index) { setInstrument(index); }

void AnwarCH04AudioProcessor::noteOn(int note, float velocity)
{
    auto it = std::find_if(voices.begin(), voices.end(), [](const Voice& v) { return !v.active; });
    if (it == voices.end()) it = voices.begin();
    *it = Voice{};
    it->note = note;
    it->frequency = juce::MidiMessage::getMidiNoteInHertz(note);
    it->velocity = velocity;
    it->active = true;
}
void AnwarCH04AudioProcessor::noteOff(int note)
{
    for (auto& v : voices) if (v.active && v.note == note) v.releasing = true;
}

float AnwarCH04AudioProcessor::nextSample(Voice& v)
{
    if (!v.active) return 0.0f;
    const int inst = instrument.load();
    const double inc = juce::MathConstants<double>::twoPi * v.frequency / sr;
    v.phase += inc;
    if (v.phase >= juce::MathConstants<double>::twoPi) v.phase -= juce::MathConstants<double>::twoPi;

    const float attack = *parameters.getRawParameterValue("attack");
    const float release = *parameters.getRawParameterValue("release");
    if (!v.releasing)
        v.envelope = juce::jmin(1.0f, v.envelope + 1.0f / (float)(juce::jmax(0.001, attack) * sr));
    else {
        v.envelope -= 1.0f / (float)(juce::jmax(0.02, release) * sr);
        if (v.envelope <= 0.0001f) { v.active = false; return 0.0f; }
    }

    const float s = (float)std::sin(v.phase);
    const float saw = (float)(2.0 * (v.phase / juce::MathConstants<double>::twoPi) - 1.0);
    const float tri = 1.0f - 4.0f * std::abs((float)(v.phase / juce::MathConstants<double>::twoPi) - 0.5f);
    float tone = s;
    switch (inst) {
        case 0: tone = 0.78f*s + 0.16f*std::sin(2.0*v.phase) + 0.06f*std::sin(3.0*v.phase); break;
        case 1: tone = 0.68f*s + 0.22f*std::sin(2.0*v.phase) + 0.10f*tri; break;
        case 2: tone = 0.55f*s + 0.35f*saw + 0.10f*std::sin(3.0*v.phase); break;
        case 3: tone = 0.72f*s + 0.28f*std::sin(2.01*v.phase); break;
        case 4: tone = 0.55f*s + 0.45f*saw; break;
        case 5: tone = 0.70f*tri + 0.30f*std::sin(2.0*v.phase); break;
    }
    return tone * v.envelope * v.velocity * 0.18f;
}

void AnwarCH04AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    for (const auto metadata : midi) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn()) noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
        else if (msg.isNoteOff()) noteOff(msg.getNoteNumber());
        else if (msg.isAllNotesOff()) for (auto& v : voices) v.releasing = true;
    }
    const float gain = *parameters.getRawParameterValue("gain");
    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        float sample = 0.0f;
        for (auto& v : voices) sample += nextSample(v);
        sample *= gain;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, i, sample);
    }
    reverbParams.wetLevel = *parameters.getRawParameterValue("reverb");
    reverb.setParameters(reverbParams);
    if (buffer.getNumChannels() >= 2)
        reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
}

juce::AudioProcessorEditor* AnwarCH04AudioProcessor::createEditor() { return new AnwarCH04AudioProcessorEditor(*this); }
void AnwarCH04AudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    state.setProperty("instrument", instrument.load(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void AnwarCH04AudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType())) {
        auto state = juce::ValueTree::fromXml(*xml);
        parameters.replaceState(state);
        setInstrument((int)state.getProperty("instrument", 0));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new AnwarCH04AudioProcessor(); }
