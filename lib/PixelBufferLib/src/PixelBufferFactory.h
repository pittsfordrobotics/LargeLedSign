#ifndef PIXELBUFFERFACTORY_H
#define PIXELBUFFERFACTORY_H

#include "PixelBuffer.h"
#include "bufferTypes/Digit1PixelBuffer.h"
#include "bufferTypes/Digit3PixelBuffer.h"
#include "bufferTypes/Digit8PixelBuffer.h"
#include "bufferTypes/LogoPixelBuffer.h"
#include "bufferTypes/TestMatrixPixelBuffer.h"
#include "bufferTypes/LegacySignPixelBuffer.h"
#include "bufferTypes/PitSignPixelBuffer.h"

class PixelBufferFactory
{
    public:
        static PixelBuffer* CreatePixelBufferForSignType(byte signType, int gpioPin);
};

#endif