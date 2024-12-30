#ifndef LEGACYSIGNPIXELBUFFER_H
#define LEGACYSIGNPIXELBUFFER_H

#include <Arduino.h>
#include <algorithm>
#include "PixelBuffer_Old.h"

class LegacySignPixelBuffer : public PixelBuffer_Old
{
    public:
        LegacySignPixelBuffer(int gpioPin);
};

#endif