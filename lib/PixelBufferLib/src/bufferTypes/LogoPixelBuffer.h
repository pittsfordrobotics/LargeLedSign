#ifndef LOGOPIXELBUFFER_H
#define LOGOPIXELBUFFER_H

#include <Arduino.h>
#include "PixelBuffer_Old.h"

class LogoPixelBuffer : public PixelBuffer_Old
{
    public:
        LogoPixelBuffer(int gpioPin);
};

#endif