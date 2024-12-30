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
        void setUpdateAmount(byte updateAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal(PixelBuffer* pixelBuffer);
        virtual void resetInternal(PixelBuffer* pixelBuffer);

    private:
        double m_percentToFill{1.0};
};

#endif