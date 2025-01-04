#ifndef RAINBOWCOLORPATTERN_H
#define RAINBOWCOLORPATTERN_H

#include <Arduino.h>
#include "NeoPixelUtils.h"
#include "ColorPattern.h"
#include <MathUtils.h>

class RainbowColorPattern : public ColorPattern
{
    public:
        RainbowColorPattern();
        virtual void reset();
        virtual unsigned long getNextColor();
        virtual void incrementOnly(unsigned int incrementAmount);
        virtual unsigned int getNumberOfParameters();

        void setHueIncrement(byte increment);

        static std::vector<String> getParameterNames();

    private:
        unsigned int m_currentHue{0};
        unsigned int m_hueIncrement{1};
};

#endif