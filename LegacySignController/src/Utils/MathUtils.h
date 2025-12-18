#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <Arduino.h>

class MathUtils {
    public:
        static int rescaleInput(int outputMin, int outputMax, byte inputValue);
        static int clamp(int value, int minValue, int maxValue);
};

#endif