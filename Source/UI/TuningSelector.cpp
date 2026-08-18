#include "TuningSelector.h"

TuningSelector::TuningSelector()
{
    // Tuning label
    tuningLabel.setText("Tuning:", juce::dontSendNotification);
    tuningLabel.setColour(juce::Label::textColourId, juce::Colour(220, 220, 225));
    tuningLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(tuningLabel);

    // Tuning combo box
    populateTuningComboBox();
    tuningComboBox.setSelectedItemIndex(0, juce::dontSendNotification);
    tuningComboBox.onChange = [this]()
    {
        int idx = tuningComboBox.getSelectedItemIndex();
        showCustomDivisions = (idx == 7); // Custom is index 7
        if (onTuningChanged)
            onTuningChanged(idx);
        resized();
        repaint();
    };
    addAndMakeVisible(tuningComboBox);

    // Root label
    rootLabel.setText("Root:", juce::dontSendNotification);
    rootLabel.setColour(juce::Label::textColourId, juce::Colour(220, 220, 225));
    rootLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(rootLabel);

    // Root combo box
    populateRootComboBox();
    rootComboBox.setSelectedItemIndex(0, juce::dontSendNotification);
    rootComboBox.onChange = [this]()
    {
        if (onRootChanged)
            onRootChanged(rootComboBox.getSelectedItemIndex());
    };
    addAndMakeVisible(rootComboBox);

    // Tuning info
    tuningInfoLabel.setText("12-TET\n12 equal divisions per octave", juce::dontSendNotification);
    tuningInfoLabel.setColour(juce::Label::textColourId, juce::Colour(180, 180, 190));
    tuningInfoLabel.setFont(juce::Font(13.0f));
    tuningInfoLabel.setJustificationType(juce::Justification::topLeft);
    addAndMakeVisible(tuningInfoLabel);

    // Note info
    noteInfoLabel.setText("", juce::dontSendNotification);
    noteInfoLabel.setColour(juce::Label::textColourId, juce::Colour(100, 200, 255));
    noteInfoLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    noteInfoLabel.setJustificationType(juce::Justification::topLeft);
    addAndMakeVisible(noteInfoLabel);

    // Custom divisions
    customDivisionsLabel.setText("Divisions/octave:", juce::dontSendNotification);
    customDivisionsLabel.setColour(juce::Label::textColourId, juce::Colour(220, 220, 225));
    customDivisionsLabel.setFont(juce::Font(13.0f));
    addAndMakeVisible(customDivisionsLabel);
    customDivisionsLabel.setVisible(false);

    customDivisionsEditor.setText("17");
    customDivisionsEditor.setInputRestrictions(3, "0123456789");
    customDivisionsEditor.onReturnKey = [this]()
    {
        int val = customDivisionsEditor.getText().getIntValue();
        if (val >= 1 && val <= 128)
        {
            if (onCustomDivisionsChanged)
                onCustomDivisionsChanged(val);
        }
    };
    customDivisionsEditor.onFocusLost = [this]()
    {
        int val = customDivisionsEditor.getText().getIntValue();
        if (val >= 1 && val <= 128)
        {
            if (onCustomDivisionsChanged)
                onCustomDivisionsChanged(val);
        }
    };
    addAndMakeVisible(customDivisionsEditor);
    customDivisionsEditor.setVisible(false);

    // Interval display
    intervalDisplay.setMultiLine(true);
    intervalDisplay.setReadOnly(true);
    intervalDisplay.setScrollbarsShown(true);
    intervalDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colour(35, 35, 40));
    intervalDisplay.setColour(juce::TextEditor::textColourId, juce::Colour(190, 190, 200));
    intervalDisplay.setColour(juce::TextEditor::outlineColourId, juce::Colour(60, 60, 70));
    intervalDisplay.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain));
    addAndMakeVisible(intervalDisplay);
}

TuningSelector::~TuningSelector()
{
}

void TuningSelector::populateTuningComboBox()
{
    auto names = TuningSystem::getAvailableTuningNames();
    for (int i = 0; i < static_cast<int>(names.size()); ++i)
        tuningComboBox.addItem(names[i], i + 1);
}

void TuningSelector::populateRootComboBox()
{
    static const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    for (int i = 0; i < 12; ++i)
        rootComboBox.addItem(noteNames[i], i + 1);
}

void TuningSelector::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(25, 25, 30));

    // Draw border
    g.setColour(juce::Colour(50, 50, 60));
    g.drawRect(getLocalBounds(), 1);
}

void TuningSelector::resized()
{
    auto bounds = getLocalBounds().reduced(12);

    auto topRow = bounds.removeFromTop(28);
    tuningLabel.setBounds(topRow.removeFromLeft(55));
    tuningComboBox.setBounds(topRow.removeFromLeft(130));
    topRow.removeFromLeft(20);
    rootLabel.setBounds(topRow.removeFromLeft(40));
    rootComboBox.setBounds(topRow.removeFromLeft(70));

    bounds.removeFromTop(8);

    // Custom divisions row
    if (showCustomDivisions)
    {
        customDivisionsLabel.setVisible(true);
        customDivisionsEditor.setVisible(true);
        auto customRow = bounds.removeFromTop(28);
        customDivisionsLabel.setBounds(customRow.removeFromLeft(120));
        customDivisionsEditor.setBounds(customRow.removeFromLeft(60));
        bounds.removeFromTop(8);
    }
    else
    {
        customDivisionsLabel.setVisible(false);
        customDivisionsEditor.setVisible(false);
    }

    // Tuning info and note info
    auto infoRow = bounds.removeFromTop(40);
    tuningInfoLabel.setBounds(infoRow.removeFromLeft(infoRow.getWidth() / 2));
    noteInfoLabel.setBounds(infoRow);

    bounds.removeFromTop(8);

    // Interval display takes remaining space
    intervalDisplay.setBounds(bounds);
}

void TuningSelector::setCurrentTuning(TuningSystem* tuning)
{
    currentTuning = tuning;
    updateTuningInfo();
    updateIntervalDisplay();
}

void TuningSelector::updateTuningInfo()
{
    if (currentTuning == nullptr)
        return;

    juce::String info;
    info << currentTuning->getName() << "\n";
    info << currentTuning->getDescription();
    tuningInfoLabel.setText(info, juce::dontSendNotification);
}

void TuningSelector::updateIntervalDisplay()
{
    if (currentTuning == nullptr)
        return;

    auto intervals = currentTuning->getIntervals();
    juce::String text;

    text << "Step   Name                    Cents     Ratio\n";
    text << "-----  ----------------------  --------  --------\n";

    for (const auto& interval : intervals)
    {
        juce::String stepStr = juce::String(interval.step).paddedRight(' ', 5);
        juce::String nameStr = juce::String(interval.name).paddedRight(' ', 22);
        juce::String centsStr = juce::String(interval.cents, 2).paddedRight(' ', 8);
        juce::String ratioStr = juce::String(interval.ratio, 5);

        text << stepStr << "  " << nameStr << "  " << centsStr << "  " << ratioStr << "\n";
    }

    intervalDisplay.setText(text);
}

void TuningSelector::setPlayedNoteInfo(int noteIndex, double frequency, double cents)
{
    juce::String info;
    info << "Step " << noteIndex << "\n";
    info << juce::String(frequency, 2) << " Hz\n";

    if (cents >= 0)
        info << "+" << juce::String(cents, 1) << " cents";
    else
        info << juce::String(cents, 1) << " cents";

    noteInfoLabel.setText(info, juce::dontSendNotification);
}

void TuningSelector::clearPlayedNoteInfo()
{
    noteInfoLabel.setText("", juce::dontSendNotification);
}

void TuningSelector::setCustomDivisions(int divisions)
{
    customDivisionsEditor.setText(juce::String(divisions), false);
}
