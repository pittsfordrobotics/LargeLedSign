#ifndef DIGIT8PIXELBUFFER_H
#define DIGIT8PIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer_Old.h"

class Digit8PixelBuffer : public PixelBuffer_Old 
{
    public:
        Digit8PixelBuffer(int gpioPin);
};

#endif