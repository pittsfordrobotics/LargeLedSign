#ifndef LOWPOWERDISPLAYPATTERN_H
#define LOWPOWERDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

class LowPowerDisplayPattern : public DisplayPattern
{
    public:
        LowPowerDisplayPattern();

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelBuffer* pixelBuffer);
        virtual void resetInternal(PixelBuffer* pixelBuffer);

    private:
        unsigned int m_iterationCount{0};
};

#endif