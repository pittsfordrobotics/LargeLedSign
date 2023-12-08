#include "MathUtils.h"

int MathUtils::rescaleInput(int outputMin, int outputMax, int inputValue)
{
    // inputValue is a byte, so inputMin = 0; inputMax = 255
    // m = (outputMax - outputMin) / (inputMax - inputMin)
    double m = (double)(outputMax - outputMin) / 255;
    // b = outputMin - m * inputMin, but inputMin is zero.
    double b = outputMin;

    return inputValue * m + b;
}