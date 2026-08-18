#pragma once

#include <string>
#include <vector>
#include <memory>

enum class TuningType
{
    TwelveTET,
    NineteenTET,
    TwentyFourTET,
    ThirtyOneTET,
    FiftyThreeTET,
    JustIntonation,
    Pythagorean,
    Custom
};

struct IntervalInfo
{
    int step;
    std::string name;
    double cents;
    double ratio;
};

class TuningSystem
{
public:
    virtual ~TuningSystem() = default;

    // Core interface
    virtual double getFrequency(int noteIndex, double rootFrequency = 440.0) const = 0;
    virtual int getDivisionsPerOctave() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TuningType getType() const = 0;

    // Interval information
    virtual std::vector<IntervalInfo> getIntervals() const = 0;

    // Utility
    virtual double getCentsFromRoot(int noteIndex) const = 0;

    // Factory
    static std::unique_ptr<TuningSystem> create(TuningType type, int customDivisions = 12);

    // Available tuning names for UI
    static std::vector<std::string> getAvailableTuningNames();
    static TuningType tuningTypeFromIndex(int index);
    static int indexFromTuningType(TuningType type);
};
