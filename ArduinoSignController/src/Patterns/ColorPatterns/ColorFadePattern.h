#ifndef COLORFADEPATTERN_H
#define COLORFADEPATTERN_H

#include <Arduino.h>
#include "Utils\NeoPixelUtils.h"
#include "ColorPattern.h"
#include "Utils\MathUtils.h"
#include <vector>

class ColorFadePattern : public ColorPattern
{
    public:
        ColorFadePattern(uint32_t color1, uint32_t color2);
        ColorFadePattern(uint32_t color1, uint32_t color2, uint32_t color3);
        ColorFadePattern(uint32_t color1, uint32_t color2, uint32_t color3, uint32_t color4);

        virtual void reset();
        virtual uint32_t getNextColor();
        virtual void incrementOnly(uint16_t incrementAmount);
        virtual uint16_t getNumberOfParameters();

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
        uint32_t m_iterationCount{0};

        std::vector<uint32_t> m_colors;
        std::vector<uint32_t> m_colorSequence;
        byte convertColorDuration(byte duration);
        byte convertFadeInOutDuration(byte duration);

        // Converts the input brightness value (0-1) to 
        // an output value (0-1) that is more visually appealing.
        double applyBrightnessGamma(double inputBrightness);
};

#endif