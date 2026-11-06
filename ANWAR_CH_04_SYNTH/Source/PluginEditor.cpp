#include "PluginEditor.h"

static const juce::Colour bg(0xff07110d), panel(0xff0d1d16), green(0xff28e58a), muted(0xff8ba99a);

AnwarCH04AudioProcessorEditor::AnwarCH04AudioProcessorEditor(AnwarCH04AudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(760, 500);
    title.setText("ANWAR CH 04 SYNTH", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(29.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, green);
    addAndMakeVisible(title);
    subtitle.setText("HYBRID INSTRUMENT  •  WINDOWS VST3", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(12.0f)));
    subtitle.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(subtitle);

    const char* names[] = { "ACOUSTIC PIANO", "ACOUSTIC GUITAR", "ELECTRIC GUITAR", "VIOLIN", "SYNTH", "REEL KEYS" };
    for (int i=0;i<6;++i) instruments.addItem(names[i], i+1);
    instruments.setSelectedId(processor.getInstrument()+1);
    instruments.onChange = [this] { processor.setInstrument(instruments.getSelectedId()-1); };
    instruments.setColour(juce::ComboBox::backgroundColourId, panel);
    instruments.setColour(juce::ComboBox::textColourId, green);
    addAndMakeVisible(instruments);

    auto setup = [this](juce::Slider& s, const juce::String& name) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 20);
        s.setName(name);
        s.setColour(juce::Slider::rotarySliderFillColourId, green);
        s.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        s.setColour(juce::Slider::textBoxOutlineColourId, panel);
        addAndMakeVisible(s);
    };
    setup(master, "MASTER"); setup(attack, "ATTACK"); setup(release, "RELEASE"); setup(reverb, "REVERB");
    masterAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "gain", master);
    attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "attack", attack);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "release", release);
    reverbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "reverb", reverb);
    styleButton(presetButton);
    presetButton.onClick = [this] { instruments.setSelectedId(1); processor.setInstrument(0); };
    addAndMakeVisible(presetButton);
    meterLabel.setText("LIVE AUDIO  /  MIDI READY", juce::dontSendNotification);
    meterLabel.setColour(juce::Label::textColourId, muted);
    meterLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(meterLabel);
    startTimerHz(30);
}
void AnwarCH04AudioProcessorEditor::styleButton(juce::TextButton& b)
{
    b.setColour(juce::TextButton::buttonColourId, panel);
    b.setColour(juce::TextButton::textColourOffId, green);
    b.setColour(juce::TextButton::buttonOnColourId, green.withAlpha(0.25f));
}
void AnwarCH04AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    g.setColour(green.withAlpha(0.16f)); g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(10), 14.0f, 1.2f);
    g.setColour(panel); g.fillRoundedRectangle(juce::Rectangle<float>(24,112,712,76),12);
    g.setColour(green.withAlpha(0.8f)); g.drawRoundedRectangle(juce::Rectangle<float>(24,112,712,76),12,1.0f);
    // Animated visualizer bars
    for (int i=0;i<52;++i) {
        float wave = 0.18f + 0.82f * std::abs(std::sin(meterPhase + i*0.23f) * std::cos(i*0.071f));
        float h = 7.0f + wave*43.0f;
        g.setColour(green.withAlpha(0.25f + wave*0.65f));
        g.fillRoundedRectangle(36.0f+i*13.0f, 177.0f-h, 7.0f, h, 3.0f);
    }
    g.setColour(muted); g.setFont(12.0f); g.drawText("INSTRUMENT", 28, 204, 130, 20, juce::Justification::left);
    g.drawText("MASTER", 52, 450, 100, 18, juce::Justification::centred);
    g.drawText("ATTACK", 230, 450, 100, 18, juce::Justification::centred);
    g.drawText("RELEASE", 408, 450, 100, 18, juce::Justification::centred);
    g.drawText("REVERB", 586, 450, 100, 18, juce::Justification::centred);
}
void AnwarCH04AudioProcessorEditor::resized()
{
    title.setBounds(28,20,500,42); subtitle.setBounds(31,62,400,22);
    instruments.setBounds(28,228,704,42);
    master.setBounds(42,280,120,160); attack.setBounds(220,280,120,160);
    release.setBounds(398,280,120,160); reverb.setBounds(576,280,120,160);
    presetButton.setBounds(570,38,160,34); meterLabel.setBounds(30,466,300,18);
}
void AnwarCH04AudioProcessorEditor::timerCallback()
{
    meterPhase += 0.12f;
    repaint(24,112,712,76);
}
