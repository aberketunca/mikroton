#pragma once

#include "TuningSystem.h"

class EqualTemperament : public TuningSystem
{
public:
    explicit EqualTemperament(int divisions = 12);

    double getFrequency(int noteIndex, double rootFrequency = 440.0) const override;
    int getDivisionsPerOctave() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    TuningType getType() const override;
    std::vector<IntervalInfo> getIntervals() const override;
    double getCentsFromRoot(int noteIndex) const override;

private:
    int divisions;
};
