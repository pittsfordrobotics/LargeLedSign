#ifndef SOLIDDISPLAYPATTERN_H
#define SOLIDDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

class SolidDisplayPattern : public DisplayPattern
{
    public:
        SolidDisplayPattern();
        SolidDisplayPattern(PixelBuffer* pixelBuffer);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal();
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal();
        virtual void resetInternal(PixelMap* pixelMap);
};

#endif