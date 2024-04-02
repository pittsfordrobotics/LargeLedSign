#include "StyleDefinitionBuilder.h"

StyleDefinition StyleDefinitionBuilder::SolidColor(ulong color)
{
    PatternData pattern;
    pattern.displayPattern = DisplayPatternType::Solid;
    pattern.colorPattern = ColorPatternType::SingleColor;
    pattern.color1 = color;

    return StyleDefinition(pattern, 1);
}

StyleDefinition  StyleDefinitionBuilder::TwoColorRight(ulong color1, ulong color2, byte duration1, byte duration2, byte speed)
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

StyleDefinition StyleDefinitionBuilder::TwoColorRandom(ulong color1, ulong color2, byte duration1, byte duration2, byte updateAmount, byte speed)
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

StyleDefinition StyleDefinitionBuilder::TwoColorCenterOut(ulong color1, ulong color2, byte duration1, byte duration2, byte speed)
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

StyleDefinition StyleDefinitionBuilder::TwoColorDigit(ulong color1, ulong color2, byte duration1, byte duration2, byte speed)
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

StyleDefinition StyleDefinitionBuilder::RainbowRight(byte hueIncrement, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::Rainbow;
    pattern.displayPattern = DisplayPatternType::Right;
    pattern.param1 = hueIncrement;
    return StyleDefinition(pattern, speed);
}

StyleDefinition StyleDefinitionBuilder::RainbowRandom(byte hueIncrement, byte updateAmount, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::Rainbow;
    pattern.displayPattern = DisplayPatternType::Random;
    pattern.param1 = hueIncrement;
    pattern.param2 = updateAmount;
    return StyleDefinition(pattern, speed);
}

StyleDefinition StyleDefinitionBuilder::RainbowLava(byte hueIncrement, byte speed)
{
    PatternData pattern;
    pattern.colorPattern = ColorPatternType::Rainbow;
    pattern.displayPattern = DisplayPatternType::Line;
    pattern.param1 = hueIncrement;
    return StyleDefinition(pattern, speed);
}
