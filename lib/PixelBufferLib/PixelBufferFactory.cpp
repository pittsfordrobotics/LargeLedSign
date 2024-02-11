#include "PixelBufferFactory.h"

PixelBuffer* PixelBufferFactory::CreatePixelBufferForSignType(byte signType, int gpioPin)
{
    PixelBuffer* pixelBuffer;

    switch (signType)
    {
        case 1:
            pixelBuffer = new Digit1PixelBuffer(gpioPin);
            break;
        case 3:
            pixelBuffer = new Digit3PixelBuffer(gpioPin);
            break;
        case 8:
            pixelBuffer = new Digit8PixelBuffer(gpioPin);
            break;
        case 15:
            pixelBuffer = new LogoPixelBuffer(gpioPin);
            break;
        case 16:
            pixelBuffer = new LegacySignPixelBuffer(gpioPin);
            break;
        default:
            pixelBuffer = new TestMatrixPixelBuffer(gpioPin);
            break;
    }

    pixelBuffer->initialize();
}

