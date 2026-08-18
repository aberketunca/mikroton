#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Tuning/TuningSystem.h"
#include "Synth/SynthVoice.h"
#include "Synth/SynthSound.h"

class MikrotonProcessor : public juce::AudioProcessor
{
public:
    MikrotonProcessor();
    ~MikrotonProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Tuning system access
    void setTuningSystem(TuningType type, int customDivisions = 12);
    TuningSystem* getTuningSystem() const { return tuningSystem.get(); }

    // Note control for the on-screen keyboard
    void noteOn(int noteIndex, float velocity = 0.8f);
    void noteOff(int noteIndex);

    // Root frequency management
    void setRootNote(int rootNoteIndex);
    int getRootNote() const { return rootNoteIndex; }
    double getRootFrequency() const { return rootFrequency; }

    // Get last played note info
    int getLastPlayedNote() const { return lastPlayedNote; }
    double getLastPlayedFrequency() const { return lastPlayedFrequency; }

    // Parameter access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Custom divisions for custom tuning
    void setCustomDivisions(int divisions);
    int getCustomDivisions() const { return customDivisions; }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::Synthesiser synth;
    std::unique_ptr<TuningSystem> tuningSystem;

    static constexpr int maxVoices = 16;

    int rootNoteIndex = 0;     // 0 = C
    double rootFrequency = 261.63; // C4 default
    int customDivisions = 12;

    // Last played note tracking
    int lastPlayedNote = -1;
    double lastPlayedFrequency = 0.0;

    // Root note frequencies (C4 through B4)
    static constexpr double rootFrequencies[12] = {
        261.63, 277.18, 293.66, 311.13, 329.63, 349.23,
        369.99, 392.00, 415.30, 440.00, 466.16, 493.88
    };

    void updateVoiceParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MikrotonProcessor)
};
