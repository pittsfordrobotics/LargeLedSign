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
    RotationCCW = 15
};

#endif
