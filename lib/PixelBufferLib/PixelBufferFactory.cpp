#include "PixelBufferFactory.h"

PixelBuffer* PixelBufferFactory::CreatePixelBufferForSignType(byte signType, int gpioPin)
{
    switch (signType)
    {
        case 1:
            return new Digit1PixelBuffer(gpioPin);
        case 3:
            return new Digit3PixelBuffer(gpioPin);
        case 8:
            return new Digit8PixelBuffer(gpioPin);
        case 15:
            return new LogoPixelBuffer(gpioPin);
        case 16:
            return new LegacySignPixelBuffer(gpioPin);
        default:
            return new TestMatrixPixelBuffer(gpioPin);
    }
}

