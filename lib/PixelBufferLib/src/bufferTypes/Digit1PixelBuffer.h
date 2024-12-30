#ifndef DIGIT1PIXELBUFFER_H
#define DIGIT1PIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer_Old.h"

class Digit1PixelBuffer : public PixelBuffer_Old 
{
    public:
        Digit1PixelBuffer(int gpioPin);
};

#endif