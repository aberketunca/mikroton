#include "TuningSystem.h"
#include "EqualTemperament.h"
#include "JustIntonation.h"
#include "Pythagorean.h"

std::unique_ptr<TuningSystem> TuningSystem::create(TuningType type, int customDivisions)
{
    switch (type)
    {
        case TuningType::TwelveTET:
            return std::make_unique<EqualTemperament>(12);
        case TuningType::NineteenTET:
            return std::make_unique<EqualTemperament>(19);
        case TuningType::TwentyFourTET:
            return std::make_unique<EqualTemperament>(24);
        case TuningType::ThirtyOneTET:
            return std::make_unique<EqualTemperament>(31);
        case TuningType::FiftyThreeTET:
            return std::make_unique<EqualTemperament>(53);
        case TuningType::JustIntonation:
            return std::make_unique<JustIntonationTuning>();
        case TuningType::Pythagorean:
            return std::make_unique<PythagoreanTuning>();
        case TuningType::Custom:
            return std::make_unique<EqualTemperament>(customDivisions);
    }
    return std::make_unique<EqualTemperament>(12);
}

std::vector<std::string> TuningSystem::getAvailableTuningNames()
{
    return {
        "12-TET",
        "Just Intonation",
        "Pythagorean",
        "19-TET",
        "24-TET",
        "31-TET",
        "53-TET",
        "Custom"
    };
}

TuningType TuningSystem::tuningTypeFromIndex(int index)
{
    switch (index)
    {
        case 0: return TuningType::TwelveTET;
        case 1: return TuningType::JustIntonation;
        case 2: return TuningType::Pythagorean;
        case 3: return TuningType::NineteenTET;
        case 4: return TuningType::TwentyFourTET;
        case 5: return TuningType::ThirtyOneTET;
        case 6: return TuningType::FiftyThreeTET;
        case 7: return TuningType::Custom;
        default: return TuningType::TwelveTET;
    }
}

int TuningSystem::indexFromTuningType(TuningType type)
{
    switch (type)
    {
        case TuningType::TwelveTET:      return 0;
        case TuningType::JustIntonation: return 1;
        case TuningType::Pythagorean:    return 2;
        case TuningType::NineteenTET:    return 3;
        case TuningType::TwentyFourTET:  return 4;
        case TuningType::ThirtyOneTET:   return 5;
        case TuningType::FiftyThreeTET:  return 6;
        case TuningType::Custom:         return 7;
    }
    return 0;
}
