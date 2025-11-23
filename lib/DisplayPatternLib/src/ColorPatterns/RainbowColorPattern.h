#ifndef RAINBOWCOLORPATTERN_H
#define RAINBOWCOLORPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "ColorPattern.h"
#include <MathUtils.h>

class RainbowColorPattern : public ColorPattern
{
    public:
        RainbowColorPattern();
        virtual void reset();
        virtual uint32_t getNextColor();
        virtual void incrementOnly(uint16_t incrementAmount);
        virtual uint16_t getNumberOfParameters();

        void setHueIncrement(byte increment);

        static std::vector<String> getParameterNames();

    private:
        uint16_t m_currentHue{0};
        uint16_t m_hueIncrement{1};
};

#endif