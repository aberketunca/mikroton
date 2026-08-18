#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Tuning/TuningSystem.h"
#include <functional>

class TuningSelector : public juce::Component
{
public:
    TuningSelector();
    ~TuningSelector() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Callbacks
    std::function<void(int tuningIndex)> onTuningChanged;
    std::function<void(int rootNoteIndex)> onRootChanged;
    std::function<void(int divisions)> onCustomDivisionsChanged;

    // Update display
    void setCurrentTuning(TuningSystem* tuning);
    void setPlayedNoteInfo(int noteIndex, double frequency, double cents);
    void clearPlayedNoteInfo();

    // Set custom divisions
    void setCustomDivisions(int divisions);

private:
    juce::ComboBox tuningComboBox;
    juce::ComboBox rootComboBox;
    juce::Label tuningLabel;
    juce::Label rootLabel;
    juce::Label tuningInfoLabel;
    juce::Label noteInfoLabel;
    juce::Label customDivisionsLabel;
    juce::TextEditor customDivisionsEditor;

    TuningSystem* currentTuning = nullptr;
    bool showCustomDivisions = false;

    // Interval display
    juce::TextEditor intervalDisplay;

    void populateTuningComboBox();
    void populateRootComboBox();
    void updateTuningInfo();
    void updateIntervalDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TuningSelector)
};
