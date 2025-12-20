#ifndef RANDOMDISPLAYPATTERN_H
#define RANDOMDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "Utils\MathUtils.h"

class RandomDisplayPattern : public DisplayPattern
{
    public:
        RandomDisplayPattern();
        void setUpdateAmount(byte updateAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        double m_percentToFill{1.0};
};

#endif