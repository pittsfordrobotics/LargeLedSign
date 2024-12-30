#ifndef TESTMATRIXPIXELBUFFER_H
#define TESTMATRIXPIXELBUFFER_H

#include <Arduino.h>
#include "PixelBuffer_Old.h"

class TestMatrixPixelBuffer : public PixelBuffer_Old
{
    public:
        TestMatrixPixelBuffer(int gpioPin);
};

#endif