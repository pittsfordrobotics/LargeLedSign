#ifndef PATTERNFACTORY_H
#define PATTERNFACTORY_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "DisplayPatterns\FireDisplayPattern.h"
#include "DisplayPatterns\SolidDisplayPattern.h"
#include "DisplayPatterns\SimpleShiftDisplayPattern.h"
#include "DisplayPatterns\RandomDisplayPattern.h"
#include "DisplayPatterns\CenterOutDisplayPattern.h"
#include "DisplayPatterns\LowPowerDisplayPattern.h"
#include "DisplayPatterns\RotationDisplayPattern.h"
#include "ColorPatterns\ColorPattern.h"
#include "ColorPatterns\ColorFadePattern.h"
#include "ColorPatterns\RainbowColorPattern.h"
#include "ColorPatterns\SingleColorPattern.h"
#include "ColorPatterns\TwoColorFadePattern.h"
#include "ColorPatterns\TwoColorPattern.h"
#include "ColorPatterns\BackgroundPlusThree.h"
#include <PixelBuffer.h>
#include "PatternData.h"
#include <vector>

class PatternFactory
{
    public:
        // Creates a DisplayPattern based on the given PatternData.
        static DisplayPattern* createForPatternData(const PatternData& patternData, PixelBuffer* pixelBuffer);

        // Retrieves a string version of all known color patterns.
        // This is in the format <PatternName>,<PatternNumber>,<#Colors>,<param1>,...,<paramN>;<PatternName>,<PatternNumber>,<#Colors>,<param1>,...,<paramN>;...
        // Ideally this method could return a vector of information structs and the caller could construct
        // the string representation later.  Since there's only one user of this method currently, just
        // take the simpler approach and calculate the string value here.
        static String getKnownColorPatterns();

        // Retrieves a string version of all known color patterns.
        // This is in the format <PatternName>,<PatternNumber>,<param1>,...,<paramN>;<PatternName>,<PatternNumber>,<param1>,...,<paramN>;...
        // Similar to getKnownColorPatterns above, this could also return a vector of information structs.
        static String getKnownDisplayPatterns();

    private:
        // Helper method to construct the appropriate ColorPattern based on the given PatternData.
        static ColorPattern* createColorPatternForPatternData(const PatternData& patternData);

        static String getColorPatternString(String patternName, ColorPatternType patternType, uint numberOfColors, std::vector<String> parameterNames);
        static String getDisplayPatternString(String patternName, DisplayPatternType patternType, std::vector<String> parameterNames);
};

#endif