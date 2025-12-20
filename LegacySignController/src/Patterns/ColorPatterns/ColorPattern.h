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
        virtual uint32_t getNextColor() = 0;

        // Increments the internal state of the color pattern by the specified number of iterations.
        virtual void incrementOnly(uint16_t incrementAmount) = 0;

        // Gets the number of colors expected by this color pattern.
        uint16_t getNumberOfColors() { return m_colorCount; }

        // Gets the number of parameters required by the color pattern.
        virtual uint16_t getNumberOfParameters() = 0;

    protected:
        uint16_t m_colorCount{0};

        // Just in-line the 'color' method here.
        uint32_t color(byte red, byte green, byte blue)
        {
            // Taken from the Adafruit_Neopixel::Color method.
            // I didn't want to add a dependency on that library for a single method, so copying it here.
            return ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
        }
};

#endif