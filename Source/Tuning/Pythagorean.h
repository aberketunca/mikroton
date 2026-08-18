#pragma once

#include "TuningSystem.h"

class PythagoreanTuning : public TuningSystem
{
public:
    PythagoreanTuning();

    double getFrequency(int noteIndex, double rootFrequency = 440.0) const override;
    int getDivisionsPerOctave() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    TuningType getType() const override;
    std::vector<IntervalInfo> getIntervals() const override;
    double getCentsFromRoot(int noteIndex) const override;

private:
    struct Ratio
    {
        int numerator;
        int denominator;
    };

    std::vector<Ratio> ratios;
    std::vector<std::string> noteNames;

    double ratioToFrequency(int noteIndex, double rootFrequency) const;
};
