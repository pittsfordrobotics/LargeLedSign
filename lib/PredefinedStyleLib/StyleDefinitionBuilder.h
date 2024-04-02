#ifndef STYLEDEFINITIONBUILDER_H
#define STYLEDEFINITIONBUILDER_H

#include <Arduino.h>
#include "StyleDefinition.h"
#include <DisplayPatternLib.h>

class StyleDefinitionBuilder
{
    public:
        static StyleDefinition SolidColor(ulong color);
        static StyleDefinition TwoColorRight(ulong color1, ulong color2, byte duration1, byte duration2, byte speed);
        static StyleDefinition TwoColorRandom(ulong color1, ulong color2, byte duration1, byte duration2, byte updateAmount, byte speed);
        static StyleDefinition TwoColorCenterOut(ulong color1, ulong color2, byte duration1, byte duration2, byte speed);
        static StyleDefinition TwoColorDigit(ulong color1, ulong color2, byte duration1, byte duration2, byte speed);
        static StyleDefinition RainbowRight(byte hueIncrement, byte speed);
        static StyleDefinition RainbowRandom(byte hueIncrement, byte updateAmound, byte speed);
        static StyleDefinition RainbowLava(byte hueIncrement, byte speed);
};      

#endif