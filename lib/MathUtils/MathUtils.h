#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cstdint>

#if !defined(byte)
typedef uint8_t byte;
#endif

class MathUtils {
    public:
        static int rescaleInput(int outputMin, int outputMax, unsigned char inputValue);
        static int clamp(int value, int minValue, int maxValue);
};

#endif