#include "EqualTemperament.h"
#include <cmath>

EqualTemperament::EqualTemperament(int divisions)
    : divisions(divisions)
{
}

double EqualTemperament::getFrequency(int noteIndex, double rootFrequency) const
{
    return rootFrequency * std::pow(2.0, static_cast<double>(noteIndex) / static_cast<double>(divisions));
}

int EqualTemperament::getDivisionsPerOctave() const
{
    return divisions;
}

std::string EqualTemperament::getName() const
{
    return std::to_string(divisions) + "-TET";
}

std::string EqualTemperament::getDescription() const
{
    return std::to_string(divisions) + " equal divisions per octave";
}

TuningType EqualTemperament::getType() const
{
    if (divisions == 12) return TuningType::TwelveTET;
    if (divisions == 19) return TuningType::NineteenTET;
    if (divisions == 24) return TuningType::TwentyFourTET;
    if (divisions == 31) return TuningType::ThirtyOneTET;
    if (divisions == 53) return TuningType::FiftyThreeTET;
    return TuningType::Custom;
}

std::vector<IntervalInfo> EqualTemperament::getIntervals() const
{
    std::vector<IntervalInfo> intervals;
    for (int i = 0; i <= divisions; ++i)
    {
        double cents = (1200.0 * i) / divisions;
        double ratio = std::pow(2.0, static_cast<double>(i) / static_cast<double>(divisions));

        std::string name = "Step " + std::to_string(i);

        // For 12-TET, provide standard note names
        if (divisions == 12)
        {
            static const std::vector<std::string> noteNames = {
                "C", "C#", "D", "D#", "E", "F",
                "F#", "G", "G#", "A", "A#", "B", "C"
            };
            name = noteNames[i];
        }

        intervals.push_back({i, name, cents, ratio});
    }
    return intervals;
}

double EqualTemperament::getCentsFromRoot(int noteIndex) const
{
    return (1200.0 * noteIndex) / divisions;
}
