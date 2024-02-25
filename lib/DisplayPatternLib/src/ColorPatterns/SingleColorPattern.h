#ifndef SINGLECOLORPATTERN_H
#define SINGLECOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"

class SingleColorPattern : public ColorPattern
{
    public:
        SingleColorPattern(ulong color);

        virtual void reset();
        virtual ulong getNextColor();
        virtual void incrementOnly(uint incrementAmount);
        virtual uint getNumberOfParameters();

        static std::vector<String> getParameterNames();

    private:
        ulong m_color{0};
};

#endif