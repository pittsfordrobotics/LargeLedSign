#ifndef TWOCOLORPATTERN_H
#define TWOCOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"
#include <MathUtils.h>
#include <vector>

class TwoColorPattern : public ColorPattern
{
    public:
        TwoColorPattern(unsigned long color1, unsigned long color2);

        virtual void reset();
        virtual unsigned long getNextColor();
        virtual void incrementOnly(unsigned int incrementAmount);
        virtual unsigned int getNumberOfParameters();

        void setColor1Duration(byte duration);
        void setColor2Duration(byte duration);

        static std::vector<String> getParameterNames();

    private:
        unsigned long m_color1{0};
        unsigned long m_color2{0};
        byte m_color1Duration{1};
        byte m_color2Duration{1};
        unsigned long m_iterationCount{0};

        std::vector<unsigned long> m_colorSequence;
        byte convertDuration(byte duration);
};

#endif