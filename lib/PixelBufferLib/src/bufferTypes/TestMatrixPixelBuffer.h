#ifndef TESTMATRIXPIXELBUFFER_H
#define TESTMATRIXPIXELBUFFER_H

#include <Arduino.h>
#include "PixelBuffer.h"

class TestMatrixPixelBuffer : public PixelBuffer
{
    public:
        TestMatrixPixelBuffer(int gpioPin);
};

#endif