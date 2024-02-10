#ifndef LEGACYSIGNPIXELBUFFER_H
#define LEGACYSIGNPIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer.h"

class LegacySignPixelBuffer : public PixelBuffer 
{
    public:
        LegacySignPixelBuffer(int gpioPin);
};

#endif