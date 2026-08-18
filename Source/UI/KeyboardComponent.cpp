#include "KeyboardComponent.h"

KeyboardComponent::KeyboardComponent()
{
    setupKeyMappings();
    setWantsKeyboardFocus(true);
    startTimer(50); // Poll keyboard state
}

KeyboardComponent::~KeyboardComponent()
{
    stopTimer();
}

void KeyboardComponent::setupKeyMappings()
{
    // Two rows of keys for playing notes
    // Bottom row: Z X C V B N M , . /
    // Top row: A S D F G H J K L ;
    // Number row: Q W E R T Y U I O P
    keyMappings = {
        {'z', 0},  {'x', 1},  {'c', 2},  {'v', 3},  {'b', 4},
        {'n', 5},  {'m', 6},  {',', 7},  {'.', 8},  {'/', 9},
        {'a', 10}, {'s', 11}, {'d', 12}, {'f', 13}, {'g', 14},
        {'h', 15}, {'j', 16}, {'k', 17}, {'l', 18}, {';', 19},
        {'q', 20}, {'w', 21}, {'e', 22}, {'r', 23}, {'t', 24},
        {'y', 25}, {'u', 26}, {'i', 27}, {'o', 28}, {'p', 29}
    };
}

void KeyboardComponent::setTuningSystem(TuningSystem* tuning)
{
    currentTuning = tuning;
    if (tuning)
    {
        divisionsPerOctave = tuning->getDivisionsPerOctave();
        // Adjust visible octaves based on divisions
        if (divisionsPerOctave <= 12)
            numOctaves = 2;
        else if (divisionsPerOctave <= 24)
            numOctaves = 2;
        else
            numOctaves = 1;
    }
    repaint();
}

void KeyboardComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(30, 30, 35));

    if (divisionsPerOctave == 12 && currentTuning &&
        (currentTuning->getType() == TuningType::TwelveTET ||
         currentTuning->getType() == TuningType::JustIntonation ||
         currentTuning->getType() == TuningType::Pythagorean))
    {
        paintPianoLayout(g);
    }
    else
    {
        paintLinearLayout(g);
    }
}

void KeyboardComponent::paintPianoLayout(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    int totalWhiteKeys = 7 * numOctaves + 1; // 7 white keys per octave + top C
    float whiteKeyWidth = bounds.getWidth() / totalWhiteKeys;
    float whiteKeyHeight = bounds.getHeight();
    float blackKeyWidth = whiteKeyWidth * 0.6f;
    float blackKeyHeight = whiteKeyHeight * 0.6f;

    // Map from step (0-11) to white key position
    static const int stepToWhite[] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
    static const bool isBlack[] = {false, true, false, true, false, false, true, false, true, false, true, false};

    // Draw white keys first
    int whiteKeyIndex = 0;
    for (int oct = 0; oct < numOctaves; ++oct)
    {
        for (int step = 0; step < 12; ++step)
        {
            if (!isBlack[step])
            {
                int noteIndex = oct * 12 + step;
                float x = bounds.getX() + whiteKeyIndex * whiteKeyWidth;
                juce::Rectangle<float> keyRect(x, bounds.getY(), whiteKeyWidth - 1, whiteKeyHeight);

                bool isActive = activeNotes.count(noteIndex) > 0;

                if (isActive)
                    g.setColour(juce::Colour(100, 200, 255));
                else
                    g.setColour(juce::Colour(240, 240, 245));

                g.fillRoundedRectangle(keyRect, 3.0f);
                g.setColour(juce::Colour(180, 180, 185));
                g.drawRoundedRectangle(keyRect, 3.0f, 0.5f);

                whiteKeyIndex++;
            }
        }
    }
    // Final C
    {
        int noteIndex = numOctaves * 12;
        float x = bounds.getX() + whiteKeyIndex * whiteKeyWidth;
        juce::Rectangle<float> keyRect(x, bounds.getY(), whiteKeyWidth - 1, whiteKeyHeight);

        bool isActive = activeNotes.count(noteIndex) > 0;

        if (isActive)
            g.setColour(juce::Colour(100, 200, 255));
        else
            g.setColour(juce::Colour(240, 240, 245));

        g.fillRoundedRectangle(keyRect, 3.0f);
        g.setColour(juce::Colour(180, 180, 185));
        g.drawRoundedRectangle(keyRect, 3.0f, 0.5f);
    }

    // Draw black keys
    whiteKeyIndex = 0;
    for (int oct = 0; oct < numOctaves; ++oct)
    {
        for (int step = 0; step < 12; ++step)
        {
            if (!isBlack[step])
            {
                if (step < 11 && isBlack[step + 1])
                {
                    int noteIndex = oct * 12 + step + 1;
                    float x = bounds.getX() + (whiteKeyIndex + 1) * whiteKeyWidth - blackKeyWidth / 2.0f;
                    juce::Rectangle<float> keyRect(x, bounds.getY(), blackKeyWidth, blackKeyHeight);

                    bool isActiveNote = activeNotes.count(noteIndex) > 0;

                    if (isActiveNote)
                        g.setColour(juce::Colour(60, 160, 220));
                    else
                        g.setColour(juce::Colour(40, 40, 50));

                    g.fillRoundedRectangle(keyRect, 2.0f);
                    g.setColour(juce::Colour(20, 20, 25));
                    g.drawRoundedRectangle(keyRect, 2.0f, 0.5f);
                }
                whiteKeyIndex++;
            }
        }
    }
}

void KeyboardComponent::paintLinearLayout(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    int totalKeys = divisionsPerOctave * numOctaves + 1;
    float keyWidth = bounds.getWidth() / totalKeys;
    float keyHeight = bounds.getHeight();

    for (int i = 0; i < totalKeys; ++i)
    {
        float x = bounds.getX() + i * keyWidth;
        juce::Rectangle<float> keyRect(x, bounds.getY(), keyWidth - 1, keyHeight);

        bool isActive = activeNotes.count(i) > 0;
        int stepInOctave = i % divisionsPerOctave;

        // Color coding: alternate between lighter and darker shades
        juce::Colour keyColour;
        if (isActive)
        {
            keyColour = juce::Colour(100, 200, 255);
        }
        else if (stepInOctave == 0)
        {
            keyColour = juce::Colour(255, 240, 220); // Root note - warm tint
        }
        else if (stepInOctave % 2 == 0)
        {
            keyColour = juce::Colour(230, 230, 235);
        }
        else
        {
            keyColour = juce::Colour(200, 200, 210);
        }

        g.setColour(keyColour);
        g.fillRoundedRectangle(keyRect, 2.0f);
        g.setColour(juce::Colour(150, 150, 160));
        g.drawRoundedRectangle(keyRect, 2.0f, 0.5f);

        // Draw step number for wider keys
        if (keyWidth > 14)
        {
            g.setColour(juce::Colour(80, 80, 90));
            g.setFont(juce::jmin(10.0f, keyWidth * 0.7f));
            g.drawText(juce::String(stepInOctave), keyRect.reduced(2).removeFromBottom(16),
                       juce::Justification::centredBottom, false);
        }
    }
}

void KeyboardComponent::resized()
{
}

int KeyboardComponent::getNoteIndexAtPosition(juce::Point<float> position) const
{
    auto bounds = getLocalBounds().toFloat().reduced(4);

    if (!bounds.contains(position))
        return -1;

    if (divisionsPerOctave == 12 && currentTuning &&
        (currentTuning->getType() == TuningType::TwelveTET ||
         currentTuning->getType() == TuningType::JustIntonation ||
         currentTuning->getType() == TuningType::Pythagorean))
    {
        // Piano layout hit detection
        int totalWhiteKeys = 7 * numOctaves + 1;
        float whiteKeyWidth = bounds.getWidth() / totalWhiteKeys;
        float blackKeyWidth = whiteKeyWidth * 0.6f;
        float blackKeyHeight = bounds.getHeight() * 0.6f;

        static const bool isBlackKey[] = {false, true, false, true, false, false, true, false, true, false, true, false};

        // Check black keys first (they're on top)
        if (position.getY() - bounds.getY() < blackKeyHeight)
        {
            int whiteIdx = 0;
            for (int oct = 0; oct < numOctaves; ++oct)
            {
                for (int step = 0; step < 12; ++step)
                {
                    if (!isBlackKey[step])
                    {
                        if (step < 11 && isBlackKey[step + 1])
                        {
                            float bx = bounds.getX() + (whiteIdx + 1) * whiteKeyWidth - blackKeyWidth / 2.0f;
                            juce::Rectangle<float> bkRect(bx, bounds.getY(), blackKeyWidth, blackKeyHeight);
                            if (bkRect.contains(position))
                                return oct * 12 + step + 1;
                        }
                        whiteIdx++;
                    }
                }
            }
        }

        // Check white keys
        int whiteIdx = 0;
        for (int oct = 0; oct < numOctaves; ++oct)
        {
            for (int step = 0; step < 12; ++step)
            {
                if (!isBlackKey[step])
                {
                    float x = bounds.getX() + whiteIdx * whiteKeyWidth;
                    juce::Rectangle<float> wkRect(x, bounds.getY(), whiteKeyWidth, bounds.getHeight());
                    if (wkRect.contains(position))
                        return oct * 12 + step;
                    whiteIdx++;
                }
            }
        }
        // Final C
        {
            float x = bounds.getX() + whiteIdx * whiteKeyWidth;
            juce::Rectangle<float> wkRect(x, bounds.getY(), whiteKeyWidth, bounds.getHeight());
            if (wkRect.contains(position))
                return numOctaves * 12;
        }
    }
    else
    {
        // Linear layout
        int totalKeys = divisionsPerOctave * numOctaves + 1;
        float keyWidth = bounds.getWidth() / totalKeys;
        float relX = position.getX() - bounds.getX();
        int index = static_cast<int>(relX / keyWidth);
        return juce::jlimit(0, totalKeys - 1, index);
    }

    return -1;
}

void KeyboardComponent::mouseDown(const juce::MouseEvent& event)
{
    int note = getNoteIndexAtPosition(event.position);
    if (note >= 0)
    {
        triggerNoteOn(note);
        mouseActiveNotes.insert(note);
        lastMouseNote = note;
    }
    grabKeyboardFocus();
}

void KeyboardComponent::mouseUp(const juce::MouseEvent& /*event*/)
{
    for (int note : mouseActiveNotes)
        triggerNoteOff(note);
    mouseActiveNotes.clear();
    lastMouseNote = -1;
}

void KeyboardComponent::mouseDrag(const juce::MouseEvent& event)
{
    int note = getNoteIndexAtPosition(event.position);
    if (note != lastMouseNote)
    {
        if (lastMouseNote >= 0)
        {
            triggerNoteOff(lastMouseNote);
            mouseActiveNotes.erase(lastMouseNote);
        }
        if (note >= 0)
        {
            triggerNoteOn(note);
            mouseActiveNotes.insert(note);
        }
        lastMouseNote = note;
    }
}

bool KeyboardComponent::keyPressed(const juce::KeyPress& /*key*/)
{
    return false; // Let keyStateChanged handle it
}

bool KeyboardComponent::keyStateChanged(bool /*isKeyDown*/)
{
    // Check all mapped keys
    std::set<int> currentlyPressed;

    for (const auto& mapping : keyMappings)
    {
        if (juce::KeyPress::isKeyCurrentlyDown(mapping.keyCode))
        {
            currentlyPressed.insert(mapping.noteOffset);
        }
    }

    // Find newly pressed keys
    for (int note : currentlyPressed)
    {
        if (keyboardActiveNotes.count(note) == 0)
        {
            triggerNoteOn(note);
            keyboardActiveNotes.insert(note);
        }
    }

    // Find released keys
    std::set<int> released;
    for (int note : keyboardActiveNotes)
    {
        if (currentlyPressed.count(note) == 0)
        {
            released.insert(note);
        }
    }
    for (int note : released)
    {
        triggerNoteOff(note);
        keyboardActiveNotes.erase(note);
    }

    return !currentlyPressed.empty() || !released.empty();
}

void KeyboardComponent::timerCallback()
{
    // Periodic keyboard state polling as backup
    if (hasKeyboardFocus(true))
        keyStateChanged(false);
}

void KeyboardComponent::triggerNoteOn(int noteIndex)
{
    activeNotes.insert(noteIndex);
    if (onNoteOn)
        onNoteOn(noteIndex, 0.8f);
    repaint();
}

void KeyboardComponent::triggerNoteOff(int noteIndex)
{
    activeNotes.erase(noteIndex);
    if (onNoteOff)
        onNoteOff(noteIndex);
    repaint();
}

void KeyboardComponent::setNoteActive(int noteIndex, bool active)
{
    if (active)
        activeNotes.insert(noteIndex);
    else
        activeNotes.erase(noteIndex);
    repaint();
}

bool KeyboardComponent::isBlackKey(int step) const
{
    static const bool blacks[] = {false, true, false, true, false, false, true, false, true, false, true, false};
    return blacks[step % 12];
}
