#include "PatternFactory.h"

DisplayPattern* PatternFactory::createForPatternData(const PatternData& patternData, PixelBuffer* pixelBuffer)
{
    ColorPattern* colorPattern = createColorPatternForPatternData(patternData);
    DisplayPattern* displayPattern;

    switch (patternData.displayPattern)
    {
        case DisplayPatternType::Up:
        {
            SimpleShiftDisplayPattern* pattern = new SimpleShiftDisplayPattern(ShiftType::Up, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::Down:
        {
            SimpleShiftDisplayPattern* pattern = new SimpleShiftDisplayPattern(ShiftType::Down, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::Right:
        {
            SimpleShiftDisplayPattern* pattern = new SimpleShiftDisplayPattern(ShiftType::Right, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::Left:
        {
            SimpleShiftDisplayPattern* pattern = new SimpleShiftDisplayPattern(ShiftType::Left, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::Digit:
        {
            SimpleShiftDisplayPattern* pattern = new SimpleShiftDisplayPattern(ShiftType::Digit, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::Random:
        {
            RandomDisplayPattern* pattern = new RandomDisplayPattern(pixelBuffer);
            pattern->setColorPattern(colorPattern);
            pattern->setUpdateAmount(patternData.param1);
            displayPattern = pattern;
            break;
        }
        default:
        {
            SolidDisplayPattern* pattern = new SolidDisplayPattern(pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
    }

    // Set any common properties (none yet?)
    return displayPattern;
}

ColorPattern* PatternFactory::createColorPatternForPatternData(const PatternData& patternData)
{
    switch (patternData.colorPattern)
    {
        case ColorPatternType::SingleColor:
            return new SingleColorPattern(patternData.color1);
        case ColorPatternType::TwoColor:
            {
                TwoColorPattern* twoColorPattern = new TwoColorPattern(patternData.color1, patternData.color2);
                twoColorPattern->setColor1Duration(patternData.param1);
                twoColorPattern->setColor2Duration(patternData.param2);
                return twoColorPattern;
            }
        case ColorPatternType::Rainbow:
            {
                RainbowColorPattern* rainbowPattern = new RainbowColorPattern();
                rainbowPattern->setHueIncrement(patternData.param1);
                return rainbowPattern;
            }
        default:
            // Unknown - use a blank color
            return new SingleColorPattern(0);
    }
}