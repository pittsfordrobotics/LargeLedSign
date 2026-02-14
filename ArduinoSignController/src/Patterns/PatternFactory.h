#ifndef PATTERNFACTORY_H
#define PATTERNFACTORY_H

#include <Arduino.h>
#include "DisplayPatterns\DisplayPattern.h"
#include "DisplayPatterns\FireDisplayPattern.h"
#include "DisplayPatterns\SolidDisplayPattern.h"
#include "DisplayPatterns\SimpleShiftDisplayPattern.h"
#include "DisplayPatterns\RandomDisplayPattern.h"
#include "DisplayPatterns\CenterOutDisplayPattern.h"
#include "DisplayPatterns\LowPowerDisplayPattern.h"
#include "DisplayPatterns\RotationDisplayPattern.h"
#include "DisplayPatterns\RadialDisplayPattern.h"
#include "ColorPatterns\ColorPattern.h"
#include "ColorPatterns\ColorFadePattern.h"
#include "ColorPatterns\RainbowColorPattern.h"
#include "ColorPatterns\SingleColorPattern.h"
#include "ColorPatterns\TwoColorPattern.h"
#include "ColorPatterns\BackgroundPlusThree.h"
#include "PatternData.h"
#include <vector>
#include <unordered_map>

class PatternFactory
{
    public:
        // Creates a DisplayPattern based on the given PatternData.
        static DisplayPattern* createForPatternData(const PatternData& patternData);

        // Retrieves a string version of all known color patterns.
        // This is in the format <PatternName>,<PatternNumber>,<#Colors>,<param1>,...,<paramN>;<PatternName>,<PatternNumber>,<#Colors>,<param1>,...,<paramN>;...
        // Ideally this method could return a vector of information structs and the caller could construct
        // the string representation later.  Since there's only one user of this method currently, just
        // take the simpler approach and calculate the string value here.
        static String getKnownColorPatterns();
        static String generateColorPatternString(const std::vector<String>& colorPatterns);

        // Retrieves a string version of all known color patterns.
        // This is in the format <PatternName>,<PatternNumber>,<param1>,...,<paramN>;<PatternName>,<PatternNumber>,<param1>,...,<paramN>;...
        // Similar to getKnownColorPatterns above, this could also return a vector of information structs.
        static String getKnownDisplayPatterns();
        static String generateDisplayPatternString(const std::vector<String>& displayPatterns);

    private:
        // Helper method to construct the appropriate ColorPattern based on the given PatternData.
        static ColorPattern* createColorPatternForPatternData(const PatternData& patternData);

        static String getColorPatternString(String patternName, ColorPatternType patternType, uint16_t numberOfColors, const std::vector<String>& parameterNames);
        static String getDisplayPatternString(String patternName, DisplayPatternType patternType, const std::vector<String>& parameterNames);

        static std::unordered_map<ColorPatternType, String> colorPatternStringLookup;
        static std::unordered_map<DisplayPatternType, String> displayPatternStringLookup;

        static void populateColorPatternStringLookup();
        static void populateDisplayPatternStringLookup();
};

#endif