#ifndef COLORPATTERN_H
#define COLORPATTERN_H

#include <Arduino.h>

class ColorPattern
{
    public:
        // Resets the color pattern to its initial state.
        virtual void reset() = 0;

        // Gets the next color in the sequence.
        virtual ulong getNextColor() = 0;
};

#endif