#ifndef LOWPOWERDISPLAYPATTERN_H
#define LOWPOWERDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"

class LowPowerDisplayPattern : public DisplayPattern
{
    public:
        LowPowerDisplayPattern();

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        uint16_t m_iterationCount{0};
};

#endif