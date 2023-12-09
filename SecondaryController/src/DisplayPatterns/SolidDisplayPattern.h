#ifndef SOLIDDISPLAYPATTERN_H
#define SOLIDDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

class SolidDisplayPattern : public DisplayPattern
{
    public:
        SolidDisplayPattern(PixelBuffer* pixelBuffer);

    protected:
        virtual void updateInternal();
        virtual void resetInternal();
};

#endif