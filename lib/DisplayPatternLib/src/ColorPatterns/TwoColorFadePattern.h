#ifndef TWOCOLORFADEPATTERN_H
#define TWOCOLORFADEPATTERN_H

#include <Arduino.h>
#include "NeoPixelUtils.h"
#include "ColorPattern.h"
#include <MathUtils.h>
#include <vector>

class TwoColorFadePattern : public ColorPattern
{
    public:
        TwoColorFadePattern(unsigned long color1, unsigned long color2);

        virtual void reset();
        virtual unsigned long getNextColor();
        virtual void incrementOnly(unsigned int incrementAmount);
        virtual unsigned int getNumberOfParameters();

        void setColorDuration(byte duration);
        void setFadeInDuration(byte duration);
        void setFadeOutDuration(byte duration);
        void setFadedDuration(byte duration);

        static std::vector<String> getParameterNames();

    private:
        unsigned long m_color1{0};
        unsigned long m_color2{0};
        byte m_colorDuration{1};
        byte m_fadeInDuration{0};
        byte m_fadeOutDuration{0};
        byte m_fadedDuration{1};
        unsigned long m_iterationCount{0};

        std::vector<unsigned long> m_colorSequence;
        byte convertColorDuration(byte duration);
        byte convertFadeInOutDuration(byte duration);

        // Converts the input brightness value (0-1) to 
        // an output value (0-1) that is more visually appealing.
        double applyBrightnessGamma(double inputBrightness);
};

#endif