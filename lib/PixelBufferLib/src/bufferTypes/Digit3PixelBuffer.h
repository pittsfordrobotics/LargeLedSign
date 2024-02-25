#ifndef DIGIT3PIXELBUFFER_H
#define DIGIT3PIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer.h"

class Digit3PixelBuffer : public PixelBuffer 
{
    public:
        Digit3PixelBuffer(int gpioPin);
};

#endif