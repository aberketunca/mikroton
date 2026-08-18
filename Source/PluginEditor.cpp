#include "PluginEditor.h"

MikrotonEditor::MikrotonEditor(MikrotonProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Title
    titleLabel.setText("Mikroton", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(240, 240, 245));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    // Keyboard
    keyboard.onNoteOn = [this](int noteIndex, float velocity)
    {
        processorRef.noteOn(noteIndex, velocity);
        double freq = processorRef.getLastPlayedFrequency();
        double cents = processorRef.getTuningSystem()->getCentsFromRoot(noteIndex);
        tuningSelector.setPlayedNoteInfo(noteIndex, freq, cents);
    };
    keyboard.onNoteOff = [this](int noteIndex)
    {
        processorRef.noteOff(noteIndex);
        tuningSelector.clearPlayedNoteInfo();
    };
    keyboard.setTuningSystem(processorRef.getTuningSystem());
    addAndMakeVisible(keyboard);

    // Tuning selector
    tuningSelector.onTuningChanged = [this](int idx) { tuningChanged(idx); };
    tuningSelector.onRootChanged = [this](int idx) { rootChanged(idx); };
    tuningSelector.onCustomDivisionsChanged = [this](int div) { customDivisionsChanged(div); };
    tuningSelector.setCurrentTuning(processorRef.getTuningSystem());
    addAndMakeVisible(tuningSelector);

    // ADSR and volume sliders
    setupSlider(volumeSlider, volumeLabel, "Volume");
    setupSlider(attackSlider, attackLabel, "A");
    setupSlider(decaySlider, decayLabel, "D");
    setupSlider(sustainSlider, sustainLabel, "S");
    setupSlider(releaseSlider, releaseLabel, "R");

    // Attachments
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), "volume", volumeSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), "release", releaseSlider);

    setSize(900, 600);
    setResizable(true, true);
    setResizeLimits(700, 450, 1400, 900);
}

MikrotonEditor::~MikrotonEditor()
{
}

void MikrotonEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(100, 200, 255));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(60, 60, 70));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(200, 200, 210));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.setFont(juce::Font(11.0f));
    label.setColour(juce::Label::textColourId, juce::Colour(180, 180, 190));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void MikrotonEditor::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(20, 20, 25));

    // Subtle gradient at top
    juce::ColourGradient gradient(juce::Colour(30, 30, 40), 0, 0,
                                  juce::Colour(20, 20, 25), 0, 80, false);
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds().removeFromTop(80));
}

void MikrotonEditor::resized()
{
    auto bounds = getLocalBounds().reduced(16);

    // Title
    auto titleArea = bounds.removeFromTop(40);
    titleLabel.setBounds(titleArea);

    bounds.removeFromTop(8);

    // Sliders row
    auto sliderArea = bounds.removeFromTop(80);
    int sliderWidth = 60;
    int numSliders = 5;
    int totalSliderWidth = sliderWidth * numSliders;
    int sliderStartX = (sliderArea.getWidth() - totalSliderWidth) / 2;

    auto sliderRow = sliderArea.withX(sliderArea.getX() + sliderStartX).withWidth(totalSliderWidth);

    struct SliderPair { juce::Slider* s; juce::Label* l; };
    SliderPair pairs[] = {
        {&volumeSlider, &volumeLabel},
        {&attackSlider, &attackLabel},
        {&decaySlider, &decayLabel},
        {&sustainSlider, &sustainLabel},
        {&releaseSlider, &releaseLabel}
    };

    for (auto& pair : pairs)
    {
        auto col = sliderRow.removeFromLeft(sliderWidth);
        pair.l->setBounds(col.removeFromTop(16));
        pair.s->setBounds(col);
    }

    bounds.removeFromTop(8);

    // Keyboard takes middle portion
    auto keyboardArea = bounds.removeFromTop(juce::jmax(120, bounds.getHeight() / 3));
    keyboard.setBounds(keyboardArea);

    bounds.removeFromTop(8);

    // Tuning selector + interval display takes remaining space
    tuningSelector.setBounds(bounds);
}

void MikrotonEditor::tuningChanged(int index)
{
    TuningType type = TuningSystem::tuningTypeFromIndex(index);
    int divisions = processorRef.getCustomDivisions();
    processorRef.setTuningSystem(type, divisions);
    keyboard.setTuningSystem(processorRef.getTuningSystem());
    tuningSelector.setCurrentTuning(processorRef.getTuningSystem());
}

void MikrotonEditor::rootChanged(int index)
{
    processorRef.setRootNote(index);
}

void MikrotonEditor::customDivisionsChanged(int divisions)
{
    processorRef.setCustomDivisions(divisions);
    processorRef.setTuningSystem(TuningType::Custom, divisions);
    keyboard.setTuningSystem(processorRef.getTuningSystem());
    tuningSelector.setCurrentTuning(processorRef.getTuningSystem());
}
