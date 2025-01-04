#ifndef COMMONSTYLES_H
#define COMMONSTYLES_H

#include <Arduino.h>
#include "StyleDefinition.h"
#include <DisplayPatternLib.h>
#include "Colors.h"

class CommonStyles
{
    public:
        static StyleDefinition SolidColor(unsigned long color);
        static StyleDefinition TwoColorRight(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte speed);
        static StyleDefinition TwoColorRandom(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte updateAmount, byte speed);
        static StyleDefinition TwoColorCenterOut(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte speed);
        static StyleDefinition TwoColorDigit(unsigned long color1, unsigned long color2, byte duration1, byte duration2, byte speed);
        static StyleDefinition RainbowRight(byte hueIncrement, byte speed);
        static StyleDefinition RainbowRandom(byte hueIncrement, byte updateAmound, byte speed);
        static StyleDefinition RainbowLava(byte hueIncrement, byte speed);
        static StyleDefinition LowPower();
};      

#endif