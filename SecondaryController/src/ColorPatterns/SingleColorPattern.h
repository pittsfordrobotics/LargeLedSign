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

    private:
        ulong m_color{0};
};

#endif