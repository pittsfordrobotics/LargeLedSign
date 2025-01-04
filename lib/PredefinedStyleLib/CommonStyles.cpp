#include "CommonStyles.h"

StyleDefinition CommonStyles::SolidColor(unsigned long color)
{
    PatternData pattern;
    pattern.displayPattern = DisplayPatternType::Solid;
    pattern.colorPattern = ColorPatternType::SingleColor;
    pattern.color1 = color;

    return StyleDefinition(pattern, 1);
}

StyleDefinition  CommonStyles::TwoColorRight(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte speed)
{
    PatternData pattern;
    pattern.displayPattern = DisplayPatternType::Right;
    pattern.colorPattern = ColorPatternType::TwoColor;
    pattern.color1 = color1;
    pattern.color2 = color2;
    pattern.param1 = duration1;
    pattern.param2 = duration2;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::TwoColorRandom(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte updateAmount, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::TwoColor;
    pattern.color1 = color1;
    pattern.color2 = color2;
    pattern.displayPattern = DisplayPatternType::Random;
    pattern.param1 = duration1;
    pattern.param2 = duration2;
    pattern.param3 = updateAmount;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::TwoColorCenterOut(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::TwoColor;
    pattern.displayPattern = DisplayPatternType::CenterOutHorizontal;
    pattern.color1 = color1;
    pattern.color2 = color2;
    pattern.param1 = duration1;
    pattern.param2 = duration2;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::TwoColorDigit(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::TwoColor;
    pattern.color1 = color1;
    pattern.color2 = color2;
    pattern.displayPattern = DisplayPatternType::Digit;
    pattern.param1 = duration1;
    pattern.param2 = duration2;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::RainbowRight(byte hueIncrement, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::Rainbow;
    pattern.displayPattern = DisplayPatternType::Right;
    pattern.param1 = hueIncrement;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::RainbowRandom(byte hueIncrement, byte updateAmount, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::Rainbow;
    pattern.displayPattern = DisplayPatternType::Random;
    pattern.param1 = hueIncrement;
    pattern.param2 = updateAmount;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::RainbowLava(byte hueIncrement, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::Rainbow;
    pattern.displayPattern = DisplayPatternType::Line;
    pattern.param1 = hueIncrement;
    return StyleDefinition(pattern, speed);
}

StyleDefinition CommonStyles::LowPower()
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::SingleColor;
    pattern.displayPattern = DisplayPatternType::LowPower;
    pattern.color1 = Colors::Red;
    return StyleDefinition(pattern, 1);
}
