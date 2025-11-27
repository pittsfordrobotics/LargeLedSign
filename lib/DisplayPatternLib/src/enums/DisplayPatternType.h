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
    LowPower = 10,
    Fire = 11,
    Fire2 = 12,
    Fire3 = 13,
    Rotation = 14,
    RotationCCW = 15,
    SpotLight = 16,
    SpotLightCCW = 17,
    CenterOutRadial = 18
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
            if (type.equalsIgnoreCase("fire"))
            {
                return DisplayPatternType::Fire;
            }
            if (type.equalsIgnoreCase("fire2"))
            {
                return DisplayPatternType::Fire2;
            }
            if (type.equalsIgnoreCase("fire3"))
            {
                return DisplayPatternType::Fire3;
            }
            if (type.equalsIgnoreCase("rotation"))
            {
                return DisplayPatternType::Rotation;
            }
            if (type.equalsIgnoreCase("rotationccw"))
            {
                return DisplayPatternType::RotationCCW;
            }
            if (type.equalsIgnoreCase("spotlight"))
            {
                return DisplayPatternType::SpotLight;
            }
            if (type.equalsIgnoreCase("spotlightccw"))
            {
                return DisplayPatternType::SpotLightCCW;
            }
            if (type.equalsIgnoreCase("centeroutradial"))
            {
                return DisplayPatternType::CenterOutRadial;
            }
            return DisplayPatternType::Solid;
        }
};

#endif
