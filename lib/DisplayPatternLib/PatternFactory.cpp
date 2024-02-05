#include "PatternFactory.h"

DisplayPattern* PatternFactory::createForPatternData(const PatternData& patternData, PixelBuffer* pixelBuffer)
{
    std::vector<byte> params;
    params.push_back(patternData.param1);
    params.push_back(patternData.param2);
    params.push_back(patternData.param3);
    params.push_back(patternData.param4);
    params.push_back(patternData.param5);
    params.push_back(patternData.param6);

    ColorPattern* colorPattern = createColorPatternForPatternData(patternData);
    DisplayPattern* displayPattern;

    uint startOfDisplayParameters = colorPattern->getNumberOfParameters();

    switch (patternData.displayPattern)
    {
        case DisplayPatternType::LowPower:
        {
            LowPowerDisplayPattern* pattern = new LowPowerDisplayPattern(pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
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
        case DisplayPatternType::Line:
        {
            SimpleShiftDisplayPattern* pattern = new SimpleShiftDisplayPattern(ShiftType::Line, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::Random:
        {
            RandomDisplayPattern* pattern = new RandomDisplayPattern(pixelBuffer);
            pattern->setColorPattern(colorPattern);
            pattern->setUpdateAmount(params[startOfDisplayParameters]);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::CenterOutVertical:
        {
            CenterOutDisplayPattern* pattern = new CenterOutDisplayPattern(CenterOutOrientation::Vertical, pixelBuffer);
            pattern->setColorPattern(colorPattern);
            displayPattern = pattern;
            break;
        }
        case DisplayPatternType::CenterOutHorizontal:
        {
            CenterOutDisplayPattern* pattern = new CenterOutDisplayPattern(CenterOutOrientation::Horizontal, pixelBuffer);
            pattern->setColorPattern(colorPattern);
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
        case ColorPatternType::TwoColorFade:
            {
                ColorFadePattern* twoColorFadePattern = new ColorFadePattern(patternData.color1, patternData.color2);
                twoColorFadePattern->setColorDuration(patternData.param1);
                twoColorFadePattern->setFadeInDuration(patternData.param2);
                twoColorFadePattern->setFadeOutDuration(patternData.param3);
                twoColorFadePattern->setFadedDuration(patternData.param4);
                return twoColorFadePattern;
            }
        case ColorPatternType::ThreeColorFade:
            {
                ColorFadePattern* threeColorFadePattern = new ColorFadePattern(patternData.color1, patternData.color2, patternData.color3);
                threeColorFadePattern->setColorDuration(patternData.param1);
                threeColorFadePattern->setFadeInDuration(patternData.param2);
                threeColorFadePattern->setFadeOutDuration(patternData.param3);
                threeColorFadePattern->setFadedDuration(patternData.param4);
                return threeColorFadePattern;
            }
        case ColorPatternType::FourColorFade:
            {
                ColorFadePattern* fourColorFadePattern = new ColorFadePattern(patternData.color1, patternData.color2, patternData.color3, patternData.color4);
                fourColorFadePattern->setColorDuration(patternData.param1);
                fourColorFadePattern->setFadeInDuration(patternData.param2);
                fourColorFadePattern->setFadeOutDuration(patternData.param3);
                fourColorFadePattern->setFadedDuration(patternData.param4);
                return fourColorFadePattern;
            }
        case ColorPatternType:: BackgroundPlusThree:
            {
                BackgroundPlusThree* bgPlusThreePattern = new BackgroundPlusThree(patternData.color1, patternData.color2, patternData.color3, patternData.color4);
                bgPlusThreePattern->setBackgroundDuration(patternData.param1);
                bgPlusThreePattern->setColor1Duration(patternData.param2);
                bgPlusThreePattern->setColor2Duration(patternData.param3);
                bgPlusThreePattern->setColor3Duration(patternData.param4);
                return bgPlusThreePattern;
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

String PatternFactory::getKnownColorPatterns()
{
    String knownPatterns;
    knownPatterns += getColorPatternString("Single Color", ColorPatternType::SingleColor, 1, SingleColorPattern::getParameterNames());
    knownPatterns += ";" + getColorPatternString("Two Color", ColorPatternType::TwoColor, 2, TwoColorPattern::getParameterNames());
    knownPatterns += ";" + getColorPatternString("Two Color Fade", ColorPatternType::TwoColorFade, 2, ColorFadePattern::getParameterNames());
    //knownPatterns += ";" + getColorPatternString("Three Color Fade", ColorPatternType::ThreeColorFade, 3, ColorFadePattern::getParameterNames());
    //knownPatterns += ";" + getColorPatternString("Four Color Fade", ColorPatternType::FourColorFade, 4, ColorFadePattern::getParameterNames());
    knownPatterns += ";" + getColorPatternString("Background+3", ColorPatternType::BackgroundPlusThree, 4, BackgroundPlusThree::getParameterNames());
    knownPatterns += ";" + getColorPatternString("Rainbow", ColorPatternType::Rainbow, 0, RainbowColorPattern::getParameterNames());

    return knownPatterns;
}

String PatternFactory::getKnownDisplayPatterns()
{
    String knownPatterns;
    knownPatterns += getDisplayPatternString("Solid", DisplayPatternType::Solid, SolidDisplayPattern::getParameterNames());
    knownPatterns += ";" + getDisplayPatternString("Right", DisplayPatternType::Right, SimpleShiftDisplayPattern::getParameterNames());
    //knownPatterns += ";" + getDisplayPatternString("Left", DisplayPatternType::Left, SimpleShiftDisplayPattern::getParameterNames());
    //knownPatterns += ";" + getDisplayPatternString("Up", DisplayPatternType::Up, SimpleShiftDisplayPattern::getParameterNames());
    knownPatterns += ";" + getDisplayPatternString("Down", DisplayPatternType::Down, SimpleShiftDisplayPattern::getParameterNames());
    knownPatterns += ";" + getDisplayPatternString("Digit", DisplayPatternType::Digit, SimpleShiftDisplayPattern::getParameterNames());
    knownPatterns += ";" + getDisplayPatternString("Random", DisplayPatternType::Random, RandomDisplayPattern::getParameterNames());
    //knownPatterns += ";" + getDisplayPatternString("CenterOut-V", DisplayPatternType::CenterOutVertical, CenterOutDisplayPattern::getParameterNames());
    knownPatterns += ";" + getDisplayPatternString("CenterOut", DisplayPatternType::CenterOutHorizontal, CenterOutDisplayPattern::getParameterNames());
    knownPatterns += ";" + getDisplayPatternString("Line", DisplayPatternType::Line, SimpleShiftDisplayPattern::getParameterNames());

    return knownPatterns;
}

String PatternFactory::getColorPatternString(String patternName, ColorPatternType patternType, uint numberOfColors, std::vector<String> parameterNames)
{
    // Format:
    // <name>,<number>,<#colors>,<param1>,...
    String patternString = patternName;
    patternString += "," + String(static_cast<byte>(patternType));
    patternString += "," + String(numberOfColors);
    
    for (uint i = 0; i < parameterNames.size(); i++)
    {
        patternString += "," + parameterNames[i];
    }

    return patternString;
}

String PatternFactory::getDisplayPatternString(String patternName, DisplayPatternType patternType, std::vector<String> parameterNames)
{
    // Format:
    // <name>,<number>,<#colors>,<param1>,...
    String patternString = patternName;
    patternString += "," + String(static_cast<byte>(patternType));
    
    for (uint i = 0; i < parameterNames.size(); i++)
    {
        patternString += "," + parameterNames[i];
    }

    return patternString;
}