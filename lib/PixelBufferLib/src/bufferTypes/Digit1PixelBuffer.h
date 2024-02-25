#ifndef DIGIT1PIXELBUFFER_H
#define DIGIT1PIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer.h"

class Digit1PixelBuffer : public PixelBuffer 
{
    public:
        Digit1PixelBuffer(int gpioPin);
};

#endif