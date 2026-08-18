#include "SynthVoice.h"
#include <cmath>

SynthVoice::SynthVoice()
{
    adsrParams.attack = 0.01f;
    adsrParams.decay = 0.1f;
    adsrParams.sustain = 0.8f;
    adsrParams.release = 0.3f;
    adsr.setParameters(adsrParams);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    currentNoteIndex = midiNoteNumber;
    gain = velocity;
    active = true;
    adsr.setSampleRate(getSampleRate());
    adsr.setParameters(adsrParams);
    adsr.noteOn();
    updatePhaseIncrement();
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        adsr.reset();
        active = false;
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
}

void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!active && !adsr.isActive())
        return;

    for (int sample = startSample; sample < startSample + numSamples; ++sample)
    {
        float envelopeValue = adsr.getNextSample();

        if (!adsr.isActive())
        {
            active = false;
            clearCurrentNote();
            break;
        }

        float currentSample = static_cast<float>(std::sin(phase * 2.0 * juce::MathConstants<double>::pi));
        currentSample *= envelopeValue * gain;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, sample, currentSample);
        }

        phase += phaseIncrement;
        if (phase >= 1.0)
            phase -= 1.0;
    }
}

void SynthVoice::setFrequency(double frequencyHz)
{
    frequency = frequencyHz;
    updatePhaseIncrement();
}

void SynthVoice::setADSR(float attack, float decay, float sustain, float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    adsr.setParameters(adsrParams);
}

void SynthVoice::setGain(float newGain)
{
    gain = newGain;
}

void SynthVoice::updatePhaseIncrement()
{
    if (getSampleRate() > 0.0)
        phaseIncrement = frequency / getSampleRate();
}
