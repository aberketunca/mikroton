#include "Pythagorean.h"
#include <cmath>

PythagoreanTuning::PythagoreanTuning()
{
    // Pythagorean tuning - based on stacking perfect fifths (3:2)
    ratios = {
        {1, 1},        // Unison (C)
        {256, 243},    // Minor second (C#/Db) - Pythagorean limma
        {9, 8},        // Major second (D)
        {32, 27},      // Minor third (D#/Eb)
        {81, 64},      // Major third (E) - Pythagorean ditone
        {4, 3},        // Perfect fourth (F)
        {729, 512},    // Tritone (F#/Gb)
        {3, 2},        // Perfect fifth (G)
        {128, 81},     // Minor sixth (G#/Ab)
        {27, 16},      // Major sixth (A)
        {16, 9},       // Minor seventh (A#/Bb)
        {243, 128}     // Major seventh (B)
    };

    noteNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
}

double PythagoreanTuning::getFrequency(int noteIndex, double rootFrequency) const
{
    return ratioToFrequency(noteIndex, rootFrequency);
}

int PythagoreanTuning::getDivisionsPerOctave() const
{
    return 12;
}

std::string PythagoreanTuning::getName() const
{
    return "Pythagorean";
}

std::string PythagoreanTuning::getDescription() const
{
    return "3-limit tuning based on stacking fifths";
}

TuningType PythagoreanTuning::getType() const
{
    return TuningType::Pythagorean;
}

std::vector<IntervalInfo> PythagoreanTuning::getIntervals() const
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

double PythagoreanTuning::getCentsFromRoot(int noteIndex) const
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

double PythagoreanTuning::ratioToFrequency(int noteIndex, double rootFrequency) const
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
