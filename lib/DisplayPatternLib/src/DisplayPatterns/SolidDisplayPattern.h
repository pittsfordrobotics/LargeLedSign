#ifndef SOLIDDISPLAYPATTERN_H
#define SOLIDDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

class SolidDisplayPattern : public DisplayPattern
{
    public:
        SolidDisplayPattern();

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelBuffer* pixelBuffer);
        virtual void resetInternal(PixelBuffer* pixelBuffer);
};

#endif