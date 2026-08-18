#include "JustIntonation.h"
#include <cmath>

JustIntonationTuning::JustIntonationTuning()
{
    // 5-limit just intonation ratios for a 12-note chromatic scale
    ratios = {
        {1, 1},      // Unison (C)
        {16, 15},    // Minor second (C#/Db)
        {9, 8},      // Major second (D)
        {6, 5},      // Minor third (D#/Eb)
        {5, 4},      // Major third (E)
        {4, 3},      // Perfect fourth (F)
        {45, 32},    // Tritone (F#/Gb)
        {3, 2},      // Perfect fifth (G)
        {8, 5},      // Minor sixth (G#/Ab)
        {5, 3},      // Major sixth (A)
        {9, 5},      // Minor seventh (A#/Bb)
        {15, 8}      // Major seventh (B)
    };

    noteNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
}

double JustIntonationTuning::getFrequency(int noteIndex, double rootFrequency) const
{
    return ratioToFrequency(noteIndex, rootFrequency);
}

int JustIntonationTuning::getDivisionsPerOctave() const
{
    return 12;
}

std::string JustIntonationTuning::getName() const
{
    return "Just Intonation";
}

std::string JustIntonationTuning::getDescription() const
{
    return "Ratio-based tuning (5-limit)";
}

TuningType JustIntonationTuning::getType() const
{
    return TuningType::JustIntonation;
}

std::vector<IntervalInfo> JustIntonationTuning::getIntervals() const
{
    std::vector<IntervalInfo> intervals;

    for (int i = 0; i < static_cast<int>(ratios.size()); ++i)
    {
        double ratio = static_cast<double>(ratios[i].numerator) / static_cast<double>(ratios[i].denominator);
        double cents = 1200.0 * std::log2(ratio);

        std::string name = noteNames[i] + " (" + std::to_string(ratios[i].numerator) + "/" + std::to_string(ratios[i].denominator) + ")";

        intervals.push_back({i, name, cents, ratio});
    }

    // Add octave
    intervals.push_back({12, "C (2/1)", 1200.0, 2.0});

    return intervals;
}

double JustIntonationTuning::getCentsFromRoot(int noteIndex) const
{
    int divisions = static_cast<int>(ratios.size());
    int octave = noteIndex / divisions;
    int step = noteIndex % divisions;

    if (step < 0)
    {
        step += divisions;
        octave -= 1;
    }

    double ratio = static_cast<double>(ratios[step].numerator) / static_cast<double>(ratios[step].denominator);
    double cents = 1200.0 * std::log2(ratio);

    return cents + (octave * 1200.0);
}

double JustIntonationTuning::ratioToFrequency(int noteIndex, double rootFrequency) const
{
    int divisions = static_cast<int>(ratios.size());
    int octave = noteIndex / divisions;
    int step = noteIndex % divisions;

    if (step < 0)
    {
        step += divisions;
        octave -= 1;
    }

    double ratio = static_cast<double>(ratios[step].numerator) / static_cast<double>(ratios[step].denominator);
    double octaveMultiplier = std::pow(2.0, octave);

    return rootFrequency * ratio * octaveMultiplier;
}
