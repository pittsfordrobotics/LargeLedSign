#ifndef PIXELBUFFERFACTORY_H
#define PIXELBUFFERFACTORY_H

#include "PixelBuffer.h"
#include "Digit1PixelBuffer.h"
#include "Digit3PixelBuffer.h"
#include "Digit8PixelBuffer.h"
#include "LogoPixelBuffer.h"
#include "TestMatrixPixelBuffer.h"
#include "LegacySignPixelBuffer.h"

class PixelBufferFactory
{
    public:
        static PixelBuffer* CreatePixelBufferForSignType(byte signType, int gpioPin);
};

#endif