#ifndef DIGIT3PIXELBUFFER_H
#define DIGIT3PIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer_Old.h"

class Digit3PixelBuffer : public PixelBuffer_Old 
{
    public:
        Digit3PixelBuffer(int gpioPin);
};

#endif