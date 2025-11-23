#ifndef SOLIDDISPLAYPATTERN_H
#define SOLIDDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"

class SolidDisplayPattern : public DisplayPattern
{
    public:
        SolidDisplayPattern();

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal(PixelMap* pixelMap);
};

#endif