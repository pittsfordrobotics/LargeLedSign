#ifndef FIREDISPLAYPATTERN_H
#define FIREDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"
#include <MathUtils.h>

class FireDisplayPattern : public DisplayPattern
{
    public:
        FireDisplayPattern(PixelBuffer* pixelBuffer);
        void setSparkingAmount(byte sparkingAmount);
        void setCoolingAmount(byte coolingAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal();
        virtual void resetInternal();

    private:
        byte m_sparking{120};
        byte m_cooling{50};
};

#endif