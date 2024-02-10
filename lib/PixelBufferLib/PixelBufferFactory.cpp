#include "PixelBufferFactory.h"

PixelBuffer* PixelBufferFactory::CreatePixelBufferForSignType(byte signType, int gpioPin)
{
    PixelBuffer* pixelBuffer;
    Digit1PixelBuffer* digit1Buffer = new Digit1PixelBuffer(gpioPin);
    pixelBuffer = (PixelBuffer*) digit1Buffer;

    return pixelBuffer;
}

