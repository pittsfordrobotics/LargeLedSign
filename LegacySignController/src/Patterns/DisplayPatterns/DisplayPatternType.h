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
    CenterOutSquare = 18,
    Radial = 19
};

class DisplayPatternTypeHelper
{
    public:
        static DisplayPatternType fromString(String type)
        {
            if (type.equalsIgnoreCase("solid") || type.equalsIgnoreCase(String((int)DisplayPatternType::Solid)))
            {
                return DisplayPatternType::Solid;
            }
            if (type.equalsIgnoreCase("right") || type.equalsIgnoreCase(String((int)DisplayPatternType::Right)))
            {
                return DisplayPatternType::Right;
            }
            if (type.equalsIgnoreCase("left") || type.equalsIgnoreCase(String((int)DisplayPatternType::Left)))
            {
                return DisplayPatternType::Left;
            }
            if (type.equalsIgnoreCase("up") || type.equalsIgnoreCase(String((int)DisplayPatternType::Up)))
            {
                return DisplayPatternType::Up;
            }
            if (type.equalsIgnoreCase("down") || type.equalsIgnoreCase(String((int)DisplayPatternType::Down)))
            {
                return DisplayPatternType::Down;
            }
            if (type.equalsIgnoreCase("digit") || type.equalsIgnoreCase(String((int)DisplayPatternType::Digit)))
            {
                return DisplayPatternType::Digit;
            }
            if (type.equalsIgnoreCase("random") || type.equalsIgnoreCase(String((int)DisplayPatternType::Random)))
            {
                return DisplayPatternType::Random;
            }
            if (type.equalsIgnoreCase("centeroutvertical") || type.equalsIgnoreCase(String((int)DisplayPatternType::CenterOutVertical)))
            {
                return DisplayPatternType::CenterOutVertical;
            }
            if (type.equalsIgnoreCase("centerouthorizontal") || type.equalsIgnoreCase(String((int)DisplayPatternType::CenterOutHorizontal)))
            {
                return DisplayPatternType::CenterOutHorizontal;
            }
            if (type.equalsIgnoreCase("line") || type.equalsIgnoreCase(String((int)DisplayPatternType::Line)))
            {
                return DisplayPatternType::Line;
            }
            if (type.equalsIgnoreCase("lowpower") || type.equalsIgnoreCase(String((int)DisplayPatternType::LowPower)))
            {
                return DisplayPatternType::LowPower;
            }
            if (type.equalsIgnoreCase("fire") || type.equalsIgnoreCase(String((int)DisplayPatternType::Fire)))
            {
                return DisplayPatternType::Fire;
            }
            if (type.equalsIgnoreCase("fire2") || type.equalsIgnoreCase(String((int)DisplayPatternType::Fire2)))
            {
                return DisplayPatternType::Fire2;
            }
            if (type.equalsIgnoreCase("fire3") || type.equalsIgnoreCase(String((int)DisplayPatternType::Fire3)))
            {
                return DisplayPatternType::Fire3;
            }
            if (type.equalsIgnoreCase("rotation") || type.equalsIgnoreCase(String((int)DisplayPatternType::Rotation)))
            {
                return DisplayPatternType::Rotation;
            }
            if (type.equalsIgnoreCase("rotationccw") || type.equalsIgnoreCase(String((int)DisplayPatternType::RotationCCW)))
            {
                return DisplayPatternType::RotationCCW;
            }
            if (type.equalsIgnoreCase("spotlight") || type.equalsIgnoreCase(String((int)DisplayPatternType::SpotLight)))
            {
                return DisplayPatternType::SpotLight;
            }
            if (type.equalsIgnoreCase("spotlightccw") || type.equalsIgnoreCase(String((int)DisplayPatternType::SpotLightCCW)))
            {
                return DisplayPatternType::SpotLightCCW;
            }
            if (type.equalsIgnoreCase("centeroutsquare") || type.equalsIgnoreCase(String((int)DisplayPatternType::CenterOutSquare)))
            {
                return DisplayPatternType::CenterOutSquare;
            }
            if (type.equalsIgnoreCase("radial") || type.equalsIgnoreCase(String((int)DisplayPatternType::Radial)))
            {
                return DisplayPatternType::Radial;
            }
            return DisplayPatternType::Solid;
        }
};

#endif
