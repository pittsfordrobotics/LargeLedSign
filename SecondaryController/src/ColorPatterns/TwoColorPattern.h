#ifndef TWOCOLORPATTERN_H
#define TWOCOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"
#include "../Math/MathUtils.h"

class TwoColorPattern : public ColorPattern
{
    public:
        TwoColorPattern(ulong color1, ulong color2, byte minimumDuration);

        virtual void reset();
        virtual ulong getNextColor();
        virtual void incrementOnly(uint incrementAmount);

        void setColor1Duration(byte duration);
        void setColor2Duration(byte duration);

    private:
        ulong m_color1{0};
        ulong m_color2{0};
        byte m_minimumDuration{1};
        byte m_color1Duration{0};
        byte m_color2Duration{0};
        ulong m_iterationCount{0};

        byte convertDuration(byte duration);
};

#endif