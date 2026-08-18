#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Custom frequency control for microtonal use
    void setFrequency(double frequencyHz);
    void setADSR(float attack, float decay, float sustain, float release);
    void setGain(float newGain);

    int getNoteIndex() const { return currentNoteIndex; }
    double getCurrentFrequency() const { return frequency; }
    bool isVoiceActive() const { return active; }

private:
    double frequency = 0.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    float gain = 0.7f;

    int currentNoteIndex = -1;
    bool active = false;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    void updatePhaseIncrement();
};
