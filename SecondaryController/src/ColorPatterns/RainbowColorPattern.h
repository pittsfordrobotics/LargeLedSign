#ifndef RAINBOWCOLORPATTERN_H
#define RAINBOWCOLORPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "ColorPattern.h"
#include "..\Math\MathUtils.h"

class RainbowColorPattern : public ColorPattern
{
    public:
        RainbowColorPattern();
        virtual void reset();
        virtual ulong getNextColor();
        virtual void incrementOnly(uint incrementAmount);
        virtual uint getNumberOfParameters();

        void setHueIncrement(byte increment);

        static std::vector<String> getParameterNames();

    private:
        uint m_currentHue{0};
        uint m_hueIncrement{1};
};

#endif