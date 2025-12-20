#ifndef TWOCOLORPATTERN_H
#define TWOCOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"
#include "Utils\MathUtils.h"
#include <vector>

class TwoColorPattern : public ColorPattern
{
    public:
        TwoColorPattern(uint32_t color1, uint32_t color2);

        virtual void reset();
        virtual uint32_t getNextColor();
        virtual void incrementOnly(uint16_t incrementAmount);
        virtual uint16_t getNumberOfParameters();

        void setColor1Duration(byte duration);
        void setColor2Duration(byte duration);

        static std::vector<String> getParameterNames();

    private:
        uint32_t m_color1{0};
        uint32_t m_color2{0};
        byte m_color1Duration{1};
        byte m_color2Duration{1};
        uint32_t m_iterationCount{0};

        std::vector<uint32_t> m_colorSequence;
        byte convertDuration(byte duration);
};

#endif