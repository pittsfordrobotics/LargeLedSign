#ifndef LOWPOWERDISPLAYPATTERN_H
#define LOWPOWERDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

class LowPowerDisplayPattern : public DisplayPattern
{
    public:
        LowPowerDisplayPattern(PixelBuffer* pixelBuffer);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal();
        virtual void updateInternal(PixelBuffer* pixelBuffer);
        virtual void resetInternal();
        virtual void resetInternal(PixelBuffer* pixelBuffer);

    private:
        uint m_iterationCount{0};
};

#endif