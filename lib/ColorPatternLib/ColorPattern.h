#ifndef COLORPATTERN_H
#define COLORPATTERN_H

#include <Arduino.h>
#include <vector>

class ColorPattern
{
    public:
        virtual ~ColorPattern() 
        {
            // ColorPattern base class doesn't have any explicit destructor logic, so just in-line the method here.
        };

        // Resets the color pattern to its initial state.
        virtual void reset() = 0;

        // Gets the next color in the sequence.
        virtual ulong getNextColor() = 0;

        // Increments the internal state of the color pattern by the specified number of iterations.
        virtual void incrementOnly(uint incrementAmount) = 0;

        // Gets the number of colors expected by this color pattern.
        uint getNumberOfColors() { return m_colorCount; }

        // Gets the number of parameters required by the color pattern.
        virtual uint getNumberOfParameters() = 0;

    protected:
        uint m_colorCount{0};

        // Just in-line the 'color' method here.
        ulong color(byte red, byte green, byte blue)
        {
            // Taken from the Adafruit_Neopixel::Color method.
            // I didn't want to add a dependency on that library for a single method, so copying it here.
            return ((ulong)red << 16) | ((ulong)green << 8) | blue;
        }
};

#endif