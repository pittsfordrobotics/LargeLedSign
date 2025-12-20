#include "MathUtils.h"

int MathUtils::rescaleInput(int outputMin, int outputMax, byte inputValue)
{
    // inputValue is a byte, so inputMin = 0; inputMax = 255
    // m = (outputMax - outputMin) / (inputMax - inputMin)
    double m = (double)(outputMax - outputMin) / 255;
    // b = outputMin - m * inputMin, but inputMin is zero.
    double b = outputMin;

    return inputValue * m + b;
}

int MathUtils::clamp(int value, int minValue, int maxValue)
{
    if (value < minValue)
    {
        return minValue;
    }
 
    if (value > maxValue)
    {
        return maxValue;
    }
 
    return value;
}