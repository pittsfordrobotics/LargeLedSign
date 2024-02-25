#include "PredefinedStyle.h"

PredefinedStyle::PredefinedStyle(String name, byte speed, PatternData patternData)
{
    m_name = name;
    m_speed = speed;
    m_patternData = patternData;
}

PredefinedStyle::PredefinedStyle(const PredefinedStyle& other)
{
    m_name = other.m_name;
    m_speed = other.m_speed;
    m_patternData = other.m_patternData;
}

byte PredefinedStyle::getSpeed()
{
    return m_speed;
}

String PredefinedStyle::getName()
{
    return m_name;
}

PatternData PredefinedStyle::getPatternData()
{
    return m_patternData;
}

PredefinedStyle& PredefinedStyle::operator=(const PredefinedStyle& other)
{
    this->m_name = other.m_name;
    this->m_speed = other.m_speed;
    this->m_patternData = other.m_patternData;
    
    return *this;
}

PredefinedStyle PredefinedStyle::getPredefinedStyle(PredefinedStyles styleName)
{
    const ulong Pink = color(230, 22, 161); // E6 16 A1
    const ulong Red = color(255, 0, 0);
    const ulong Blue = color(0, 0, 255);

    switch (styleName)
    {
        case PredefinedStyles::LowPower:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::SingleColor;
            pattern.displayPattern = DisplayPatternType::LowPower;
            pattern.color1 = Red;
            return PredefinedStyle{"Low Power", 1, pattern};
        }
        case PredefinedStyles::Pink_Solid:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::SingleColor;
            pattern.displayPattern = DisplayPatternType::Solid;
            pattern.color1 = Pink;
            return PredefinedStyle{"Solid Pink", 1, pattern};
        }
        case PredefinedStyles::RedPink_Right:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.displayPattern = DisplayPatternType::Right;
            pattern.color1 = Red;
            pattern.color2 = Pink;
            pattern.param1 = 40; // Duration, 0-255, scaled to 1-50
            pattern.param2 = 15; // Duration, 0-255, scaled to 1-50
            return PredefinedStyle{"Red-Pink", 65, pattern};
        }
        case PredefinedStyles::BluePink_Right:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.displayPattern = DisplayPatternType::Right;
            pattern.color1 = Blue;
            pattern.color2 = Pink;
            pattern.param1 = 40; // Duration, 0-255, scaled to 1-50
            pattern.param2 = 15; // Duration, 0-255, scaled to 1-50
            return PredefinedStyle{"Blue-Pink", 65, pattern};
        }
        case PredefinedStyles::Rainbow_Right:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::Rainbow;
            pattern.displayPattern = DisplayPatternType::Right;
            pattern.param1 = 120; // Hue increment, 0-255, scaled to 5-1000
            return PredefinedStyle{"Rainbow", 215, pattern};
        }
        case PredefinedStyles::RedPink_Random_v1:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.color1 = Red;
            pattern.color2 = Pink;
            pattern.displayPattern = DisplayPatternType::Random;
            pattern.param1 = 255; // Duration, 0-255, scaled to 1-50
            pattern.param2 = 255; // Duration, 0-255, scaled to 1-50
            pattern.param3 = 50; // Percent of pixels to update each iteration, 0-255, scaled to 1-50.
            return PredefinedStyle{"Red-Pink Random", 255, pattern};
        }
        case PredefinedStyles::BluePink_Random_v1:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.color1 = Blue;
            pattern.color2 = Pink;
            pattern.displayPattern = DisplayPatternType::Random;
            pattern.param1 = 255; // Duration, 0-255, scaled to 1-50
            pattern.param2 = 255; // Duration, 0-255, scaled to 1-50
            pattern.param3 = 50; // Percent of pixels to update each iteration, 0-255, scaled to 1-50.
            return PredefinedStyle{"Blue-Pink Random", 255, pattern};
        }
        case PredefinedStyles::Rainbow_Random_v1:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::Rainbow;
            pattern.displayPattern = DisplayPatternType::Random;
            pattern.param1 = 255; // Hue increment, 0-255, scaled to 5-1000
            pattern.param2 = 30;  // Percent of pixels to update each iteration, 0-255, scaled to 1-50.
            return PredefinedStyle{"Rainbow Random", 245, pattern};
        }
        case PredefinedStyles::RedPink_CenterOut:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColorFade;
            pattern.displayPattern = DisplayPatternType::CenterOutHorizontal;
            pattern.color1 = Red;
            pattern.color2 = Pink;
            pattern.param1 = 19; // color duration
            pattern.param2 = 0;  // fade-in duration
            pattern.param3 = 65; // fade-out duration
            pattern.param4 = 22; // faded duration
            return PredefinedStyle{"Blue-Pink Center-Out", 196, pattern};
        }
        case PredefinedStyles::BluePink_CenterOut:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColorFade;
            pattern.displayPattern = DisplayPatternType::CenterOutHorizontal;
            pattern.color1 = Blue;
            pattern.color2 = Pink;
            pattern.param1 = 19; // color duration
            pattern.param2 = 0;  // fade-in duration
            pattern.param3 = 65; // fade-out duration
            pattern.param4 = 22; // faded duration
            return PredefinedStyle{"Blue-Pink Center-Out", 196, pattern};
        }
        case PredefinedStyles::RedPink_Digit:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.displayPattern = DisplayPatternType::Digit;
            pattern.color1 = Red;
            pattern.color2 = Pink;
            pattern.param1 = 20; // color1 duration
            pattern.param2 = 20; // color2 duration
            return PredefinedStyle{"Red-Pink Digit", 150, pattern};
        }
        case PredefinedStyles::BluePink_Digit:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.displayPattern = DisplayPatternType::Digit;
            pattern.color1 = Blue;
            pattern.color2 = Pink;
            pattern.param1 = 20; // color1 duration
            pattern.param2 = 20; // color2 duration
            return PredefinedStyle{"Blue-Pink Digit", 150, pattern};
        }
        default:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::SingleColor;
            pattern.displayPattern = DisplayPatternType::Solid;
            pattern.color1 = Pink;
            return PredefinedStyle{"Solid Pink", 1, pattern};
        }
    }
}

ulong PredefinedStyle::color(byte red, byte green, byte blue)
{
    // Taken from the Adafruit_Neopixel::Color method.
    // I didn't want to add a dependency on that library for a single method, so copying it here.
    return ((ulong)red << 16) | ((ulong)green << 8) | blue;
}