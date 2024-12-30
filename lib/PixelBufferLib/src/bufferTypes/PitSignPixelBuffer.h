#ifndef PITSIGNPIXELBUFFER_H
#define PITSIGNPIXELBUFFER_H

#include <Arduino.h>
#include "PixelBuffer_Old.h"

class PitSignPixelBuffer : public PixelBuffer_Old
{
    public:
        PitSignPixelBuffer(int gpioPin);
};

#endif