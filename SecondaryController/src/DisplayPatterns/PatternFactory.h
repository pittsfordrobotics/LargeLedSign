#ifndef PATTERNFACTORY_H
#define PATTERNFACTORY_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "SolidDisplayPattern.h"
#include "SimpleShiftDisplayPattern.h"
#include "RandomDisplayPattern.h"
#include "ColorPatterns/ColorPattern.h"
#include "ColorPatterns/SingleColorPattern.h"
#include "ColorPatterns/TwoColorPattern.h"
#include "ColorPatterns/RainbowColorPattern.h"
#include "PixelBuffer.h"
#include <PatternData.h>

class PatternFactory
{
    public:
        static DisplayPattern* CreateForPatternData(const PatternData& patternData, PixelBuffer* pixelBuffer);

    private:
        static ColorPattern* CreateColorPatternForPatternData(const PatternData& patternData);
};

#endif