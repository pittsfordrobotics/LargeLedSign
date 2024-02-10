#ifndef DIGIT8PIXELBUFFER_H
#define DIGIT8PIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer.h"

class Digit8PixelBuffer : public PixelBuffer 
{
    public:
        Digit8PixelBuffer(int gpioPin);
};

#endif