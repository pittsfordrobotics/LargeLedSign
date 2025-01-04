#ifndef DISPLAYPATTERNTYPE_H
#define DISPLAYPATTERNTYPE_H

#include <Arduino.h>

enum class DisplayPatternType : byte
{
    Solid = 0,
    Right = 1,
    Left = 2,
    Up = 3,
    Down = 4,
    Digit = 5,
    Random = 6,
	CenterOutVertical = 7,
	CenterOutHorizontal = 8,
	Line = 9,
    LowPower = 10
};

class DisplayPatternTypeHelper
{
    public:
        static DisplayPatternType fromString(String type)
        {
            if (type.equalsIgnoreCase("solid"))
            {
                return DisplayPatternType::Solid;
            }
            if (type.equalsIgnoreCase("right"))
            {
                return DisplayPatternType::Right;
            }
            if (type.equalsIgnoreCase("left"))
            {
                return DisplayPatternType::Left;
            }
            if (type.equalsIgnoreCase("up"))
            {
                return DisplayPatternType::Up;
            }
            if (type.equalsIgnoreCase("down"))
            {
                return DisplayPatternType::Down;
            }
            if (type.equalsIgnoreCase("digit"))
            {
                return DisplayPatternType::Digit;
            }
            if (type.equalsIgnoreCase("random"))
            {
                return DisplayPatternType::Random;
            }
            if (type.equalsIgnoreCase("centeroutvertical"))
            {
                return DisplayPatternType::CenterOutVertical;
            }
            if (type.equalsIgnoreCase("centerouthorizontal"))
            {
                return DisplayPatternType::CenterOutHorizontal;
            }
            if (type.equalsIgnoreCase("line"))
            {
                return DisplayPatternType::Line;
            }
            if (type.equalsIgnoreCase("lowpower"))
            {
                return DisplayPatternType::LowPower;
            }
            return DisplayPatternType::Solid;
        }
};

#endif
