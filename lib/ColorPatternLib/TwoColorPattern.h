#ifndef TWOCOLORPATTERN_H
#define TWOCOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"
#include <MathUtils.h>
#include <vector>

class TwoColorPattern : public ColorPattern
{
    public:
        TwoColorPattern(ulong color1, ulong color2);

        virtual void reset();
        virtual ulong getNextColor();
        virtual void incrementOnly(uint incrementAmount);
        virtual uint getNumberOfParameters();

        void setColor1Duration(byte duration);
        void setColor2Duration(byte duration);

        static std::vector<String> getParameterNames();

    private:
        ulong m_color1{0};
        ulong m_color2{0};
        byte m_color1Duration{1};
        byte m_color2Duration{1};
        ulong m_iterationCount{0};

        std::vector<ulong> m_colorSequence;
        byte convertDuration(byte duration);
};

#endif