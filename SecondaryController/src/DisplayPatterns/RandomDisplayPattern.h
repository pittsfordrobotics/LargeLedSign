#ifndef RANDOMDISPLAYPATTERN_H
#define RANDOMDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"
#include "Math/MathUtils.h"

class RandomDisplayPattern : public DisplayPattern
{
    public:
        RandomDisplayPattern(PixelBuffer* pixelBuffer);
        void setUpdateAmount(byte updateAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal();
        virtual void resetInternal();

    private:
        double m_percentToFill{1.0};
};

#endif