#include "PredefinedStyle.h"

PredefinedStyle::PredefinedStyle(String name, byte brightness, byte speed, PatternData patternData)
{
    m_name = name;
    m_brightness = brightness;
    m_speed = speed;
    m_patternData = patternData;
}

PredefinedStyle::PredefinedStyle(const PredefinedStyle& other)
{
    m_name = other.m_name;
    m_brightness = other.m_brightness;
    m_speed = other.m_speed;
    m_patternData = other.m_patternData;
}

byte PredefinedStyle::getBrightness()
{
    return m_brightness;
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
    this->m_brightness = other.m_brightness;
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
        case PredefinedStyles::Pink_Solid:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::SingleColor;
            pattern.displayPattern = DisplayPatternType::Solid;
            pattern.color1 = Pink;
            return PredefinedStyle{"Solid Pink", DEFAULT_BRIGHTNESS, 1, pattern};
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
            return PredefinedStyle{"Red-Pink", DEFAULT_BRIGHTNESS, 65, pattern};
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
            return PredefinedStyle{"Blue-Pink", DEFAULT_BRIGHTNESS, 65, pattern};
        }
        case PredefinedStyles::Rainbow_Right:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::Rainbow;
            pattern.displayPattern = DisplayPatternType::Right;
            pattern.param1 = 120; // Hue increment, 0-255, scaled to 5-1000
            return PredefinedStyle{"Rainbow", DEFAULT_BRIGHTNESS, 215, pattern};
        }
        case PredefinedStyles::RedPink_Random:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.color1 = Red;
            pattern.color2 = Pink;
            pattern.displayPattern = DisplayPatternType::Random;
            pattern.param1 = 30; // Duration, 0-255, scaled to 1-50
            pattern.param2 = 10; // Duration, 0-255, scaled to 1-50
            pattern.param3 = 15; // Percent of pixels to update each iteration, 0-255, scaled to 1-50.
            return PredefinedStyle{"Red-Pink Random", DEFAULT_BRIGHTNESS, 200, pattern};
        }
        case PredefinedStyles::BluePink_Random:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::TwoColor;
            pattern.color1 = Blue;
            pattern.color2 = Pink;
            pattern.displayPattern = DisplayPatternType::Random;
            pattern.param1 = 30; // Duration, 0-255, scaled to 1-50
            pattern.param2 = 10; // Duration, 0-255, scaled to 1-50
            pattern.param3 = 15; // Percent of pixels to update each iteration, 0-255, scaled to 1-50.
            return PredefinedStyle{"Blue-Pink Random", DEFAULT_BRIGHTNESS, 200, pattern};
        }
        case PredefinedStyles::Rainbow_Random:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::Rainbow;
            pattern.displayPattern = DisplayPatternType::Random;
            pattern.param1 = 120; // Hue increment, 0-255, scaled to 5-1000
            pattern.param2 = 15;  // Percent of pixels to update each iteration, 0-255, scaled to 1-50.
            return PredefinedStyle{"Rainbow Random", DEFAULT_BRIGHTNESS, 200, pattern};
        }
        default:
        {
            PatternData pattern;
            pattern.colorPattern = ColorPatternType::SingleColor;
            pattern.displayPattern = DisplayPatternType::Solid;
            pattern.color1 = Pink;
            return PredefinedStyle{"Solid Pink", DEFAULT_BRIGHTNESS, 1, pattern};
        }
    }
}

ulong PredefinedStyle::color(byte red, byte green, byte blue)
{
    // Taken from the Adafruit_Neopixel::Color method.
    // I didn't want to add a dependency on that library for a single method, so copying it here.
    return ((ulong)red << 16) | ((ulong)green << 8) | blue;
}