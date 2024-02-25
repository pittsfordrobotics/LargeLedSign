#ifndef COLORFADEPATTERN_H
#define COLORFADEPATTERN_H

#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "ColorPattern.h"
#include <MathUtils.h>
#include <vector>

class ColorFadePattern : public ColorPattern
{
    public:
        ColorFadePattern(ulong color1, ulong color2);
        ColorFadePattern(ulong color1, ulong color2, ulong color3);
        ColorFadePattern(ulong color1, ulong color2, ulong color3, ulong color4);

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
        byte m_colorDuration{1};
        byte m_fadeInDuration{0};
        byte m_fadeOutDuration{0};
        byte m_fadedDuration{1};
        ulong m_iterationCount{0};

        std::vector<ulong> m_colors;
        std::vector<ulong> m_colorSequence;
        byte convertColorDuration(byte duration);
        byte convertFadeInOutDuration(byte duration);

        // Converts the input brightness value (0-1) to 
        // an output value (0-1) that is more visually appealing.
        double applyBrightnessGamma(double inputBrightness);
};

#endif