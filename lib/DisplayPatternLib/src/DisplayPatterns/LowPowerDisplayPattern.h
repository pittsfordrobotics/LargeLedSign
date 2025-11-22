#ifndef LOWPOWERDISPLAYPATTERN_H
#define LOWPOWERDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

class LowPowerDisplayPattern : public DisplayPattern
{
    public:
        LowPowerDisplayPattern();
        LowPowerDisplayPattern(PixelBuffer* pixelBuffer);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal();
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal();
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        uint m_iterationCount{0};
};

#endif