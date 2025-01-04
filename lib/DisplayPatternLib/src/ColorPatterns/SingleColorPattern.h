#ifndef SINGLECOLORPATTERN_H
#define SINGLECOLORPATTERN_H

#include <Arduino.h>
#include "ColorPattern.h"

class SingleColorPattern : public ColorPattern
{
    public:
        SingleColorPattern(unsigned long color);

        virtual void reset();
        virtual unsigned long getNextColor();
        virtual void incrementOnly(unsigned int incrementAmount);
        virtual unsigned int getNumberOfParameters();

        static std::vector<String> getParameterNames();

    private:
        unsigned long m_color{0};
};

#endif