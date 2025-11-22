#ifndef RANDOMDISPLAYPATTERN_H
#define RANDOMDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"
#include <MathUtils.h>

class RandomDisplayPattern : public DisplayPattern
{
    public:
        RandomDisplayPattern();
        RandomDisplayPattern(PixelBuffer* pixelBuffer);
        void setUpdateAmount(byte updateAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal();
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal();
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        double m_percentToFill{1.0};
};

#endif