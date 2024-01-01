#ifndef TWOCOLORFADEPATTERN_H
#define TWOCOLORFADEPATTERN_H

#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "ColorPattern.h"
#include "../Math/MathUtils.h"
#include <vector>

class TwoColorFadePattern : public ColorPattern
{
    public:
        TwoColorFadePattern(ulong color1, ulong color2);

        virtual void reset();
        virtual ulong getNextColor();
        virtual void incrementOnly(uint incrementAmount);
        virtual uint getNumberOfParameters();

        void setColorDuration(byte duration);
        void setFadeInDuration(byte duration);
        void setFadeOutDuration(byte duration);
        void setFadedDuration(byte duration);

        static std::vector<String> getParameterNames();

    private:
        ulong m_color1{0};
        ulong m_color2{0};
        byte m_colorDuration{1};
        byte m_fadeInDuration{0};
        byte m_fadeOutDuration{0};
        byte m_fadedDuration{1};
        ulong m_iterationCount{0};

        std::vector<ulong> m_colorSequence;
        byte convertColorDuration(byte duration);
        byte convertFadeInOutDuration(byte duration);

        // Converts the input brightness value (0-1) to 
        // an output value (0-1) that is more visually appealing.
        double applyBrightnessGamma(double inputBrightness);
};

#endif