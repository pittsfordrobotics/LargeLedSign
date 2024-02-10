#ifndef LOGOPIXELBUFFER_H
#define LOGOPIXELBUFFER_H

#include <Arduino.h>
#include "PixelBuffer.h"

class LogoPixelBuffer : public PixelBuffer
{
    public:
        LogoPixelBuffer(int gpioPin);
};

#endif