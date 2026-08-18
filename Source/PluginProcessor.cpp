#include "PluginProcessor.h"
#include "PluginEditor.h"

MikrotonProcessor::MikrotonProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize with 12-TET
    tuningSystem = TuningSystem::create(TuningType::TwelveTET);

    // Add voices
    for (int i = 0; i < maxVoices; ++i)
        synth.addVoice(new SynthVoice());

    synth.addSound(new SynthSound());
}

MikrotonProcessor::~MikrotonProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MikrotonProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"volume", 1}, "Volume", 0.0f, 1.0f, 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"attack", 1}, "Attack", 0.001f, 2.0f, 0.01f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"decay", 1}, "Decay", 0.001f, 2.0f, 0.1f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"sustain", 1}, "Sustain", 0.0f, 1.0f, 0.8f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"release", 1}, "Release", 0.001f, 3.0f, 0.3f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{"rootNote", 1}, "Root Note", 0, 11, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{"tuning", 1}, "Tuning", 0, 7, 0));

    return {params.begin(), params.end()};
}

void MikrotonProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setADSR(
                *apvts.getRawParameterValue("attack"),
                *apvts.getRawParameterValue("decay"),
                *apvts.getRawParameterValue("sustain"),
                *apvts.getRawParameterValue("release")
            );
        }
    }
}

void MikrotonProcessor::releaseResources()
{
}

bool MikrotonProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void MikrotonProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    updateVoiceParameters();

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Apply master volume
    float volume = *apvts.getRawParameterValue("volume");
    buffer.applyGain(volume);
}

void MikrotonProcessor::updateVoiceParameters()
{
    float attack = *apvts.getRawParameterValue("attack");
    float decay = *apvts.getRawParameterValue("decay");
    float sustain = *apvts.getRawParameterValue("sustain");
    float release = *apvts.getRawParameterValue("release");

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setADSR(attack, decay, sustain, release);
        }
    }
}

void MikrotonProcessor::setTuningSystem(TuningType type, int divisions)
{
    customDivisions = divisions;
    tuningSystem = TuningSystem::create(type, divisions);
}

void MikrotonProcessor::noteOn(int noteIndex, float velocity)
{
    if (tuningSystem == nullptr)
        return;

    double frequency = tuningSystem->getFrequency(noteIndex, rootFrequency);
    lastPlayedNote = noteIndex;
    lastPlayedFrequency = frequency;

    // Find a free voice and set its frequency
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            if (!voice->isVoiceActive())
            {
                voice->setFrequency(frequency);
                // Use noteIndex mapped to MIDI range for voice management
                int midiNote = juce::jlimit(0, 127, noteIndex + 60);
                synth.noteOn(1, midiNote, velocity);
                return;
            }
        }
    }

    // If no free voice, steal the first one
    if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(0)))
    {
        voice->setFrequency(frequency);
        int midiNote = juce::jlimit(0, 127, noteIndex + 60);
        synth.noteOn(1, midiNote, velocity);
    }
}

void MikrotonProcessor::noteOff(int noteIndex)
{
    int midiNote = juce::jlimit(0, 127, noteIndex + 60);
    synth.noteOff(1, midiNote, 0.0f, true);
}

void MikrotonProcessor::setRootNote(int index)
{
    rootNoteIndex = juce::jlimit(0, 11, index);
    rootFrequency = rootFrequencies[rootNoteIndex];
}

void MikrotonProcessor::setCustomDivisions(int divisions)
{
    customDivisions = juce::jlimit(1, 128, divisions);
    if (tuningSystem && tuningSystem->getType() == TuningType::Custom)
    {
        tuningSystem = TuningSystem::create(TuningType::Custom, customDivisions);
    }
}

juce::AudioProcessorEditor* MikrotonProcessor::createEditor()
{
    return new MikrotonEditor(*this);
}

bool MikrotonProcessor::hasEditor() const
{
    return true;
}

const juce::String MikrotonProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MikrotonProcessor::acceptsMidi() const { return true; }
bool MikrotonProcessor::producesMidi() const { return false; }
bool MikrotonProcessor::isMidiEffect() const { return false; }
double MikrotonProcessor::getTailLengthSeconds() const { return 0.0; }

int MikrotonProcessor::getNumPrograms() { return 1; }
int MikrotonProcessor::getCurrentProgram() { return 0; }
void MikrotonProcessor::setCurrentProgram(int) {}
const juce::String MikrotonProcessor::getProgramName(int) { return {}; }
void MikrotonProcessor::changeProgramName(int, const juce::String&) {}

void MikrotonProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MikrotonProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MikrotonProcessor();
}
