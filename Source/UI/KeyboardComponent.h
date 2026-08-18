#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Tuning/TuningSystem.h"
#include <functional>
#include <set>

class KeyboardComponent : public juce::Component,
                          public juce::Timer
{
public:
    KeyboardComponent();
    ~KeyboardComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    bool keyPressed(const juce::KeyPress& key) override;
    bool keyStateChanged(bool isKeyDown) override;

    void timerCallback() override;

    // Set the tuning system to determine key layout
    void setTuningSystem(TuningSystem* tuning);

    // Callbacks
    std::function<void(int noteIndex, float velocity)> onNoteOn;
    std::function<void(int noteIndex)> onNoteOff;

    // Set active note highlighting
    void setNoteActive(int noteIndex, bool active);

private:
    TuningSystem* currentTuning = nullptr;
    int divisionsPerOctave = 12;
    int numOctaves = 2;
    int startOctave = 0; // relative to root

    std::set<int> activeNotes;
    std::set<int> mouseActiveNotes;
    std::set<int> keyboardActiveNotes;

    // Computer keyboard mapping
    struct KeyMapping
    {
        int keyCode;
        int noteOffset;
    };
    std::vector<KeyMapping> keyMappings;
    std::set<int> pressedKeys;

    void setupKeyMappings();
    int getNoteIndexAtPosition(juce::Point<float> position) const;
    juce::Rectangle<float> getKeyBounds(int noteIndex) const;
    bool isBlackKey(int step) const;

    // For 12-TET style layout
    void paintPianoLayout(juce::Graphics& g);
    // For non-12 layouts
    void paintLinearLayout(juce::Graphics& g);

    void triggerNoteOn(int noteIndex);
    void triggerNoteOff(int noteIndex);

    int lastMouseNote = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardComponent)
};
