#ifndef COLORFADEPATTERN_H
#define COLORFADEPATTERN_H

#include <Arduino.h>
#include "NeoPixelUtils.h"
#include "ColorPattern.h"
#include <MathUtils.h>
#include <vector>

class ColorFadePattern : public ColorPattern
{
    public:
        ColorFadePattern(unsigned long color1, unsigned long color2);
        ColorFadePattern(unsigned long color1, unsigned long color2, unsigned long color3);
        ColorFadePattern(unsigned long color1, unsigned long color2, unsigned long color3, unsigned long color4);

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
        byte m_colorDuration{1};
        byte m_fadeInDuration{0};
        byte m_fadeOutDuration{0};
        byte m_fadedDuration{1};
        unsigned long m_iterationCount{0};

        std::vector<unsigned long> m_colors;
        std::vector<unsigned long> m_colorSequence;
        byte convertColorDuration(byte duration);
        byte convertFadeInOutDuration(byte duration);

        // Converts the input brightness value (0-1) to 
        // an output value (0-1) that is more visually appealing.
        double applyBrightnessGamma(double inputBrightness);
};

#endif