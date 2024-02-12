#ifndef PITSIGNPIXELBUFFER_H
#define PITSIGNPIXELBUFFER_H

#include <Arduino.h>
#include "PixelBuffer.h"

class PitSignPixelBuffer : public PixelBuffer
{
    public:
        PitSignPixelBuffer(int gpioPin);
};

#endif