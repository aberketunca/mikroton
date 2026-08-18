#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "UI/KeyboardComponent.h"
#include "UI/TuningSelector.h"

class MikrotonEditor : public juce::AudioProcessorEditor
{
public:
    explicit MikrotonEditor(MikrotonProcessor& processor);
    ~MikrotonEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    MikrotonProcessor& processorRef;

    // UI Components
    juce::Label titleLabel;
    KeyboardComponent keyboard;
    TuningSelector tuningSelector;

    // ADSR sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Slider volumeSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    juce::Label volumeLabel;

    // APVTS attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);
    void tuningChanged(int index);
    void rootChanged(int index);
    void customDivisionsChanged(int divisions);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MikrotonEditor)
};
