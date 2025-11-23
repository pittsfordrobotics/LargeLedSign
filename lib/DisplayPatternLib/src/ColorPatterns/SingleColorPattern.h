#ifndef SINGLECOLORPATTERN_H
#define SINGLECOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"

class SingleColorPattern : public ColorPattern
{
    public:
        SingleColorPattern(ulong color);

        virtual void reset();
        virtual uint32_t getNextColor();
        virtual void incrementOnly(uint16_t incrementAmount);
        virtual uint16_t getNumberOfParameters();

        static std::vector<String> getParameterNames();

    private:
        uint32_t m_color{0};
};

#endif