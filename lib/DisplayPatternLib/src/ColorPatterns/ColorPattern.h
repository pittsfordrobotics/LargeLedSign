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
        virtual unsigned long getNextColor() = 0;

        // Increments the internal state of the color pattern by the specified number of iterations.
        virtual void incrementOnly(unsigned int incrementAmount) = 0;

        // Gets the number of colors expected by this color pattern.
        unsigned int getNumberOfColors() { return m_colorCount; }

        // Gets the number of parameters required by the color pattern.
        virtual unsigned int getNumberOfParameters() = 0;

    protected:
        unsigned int m_colorCount{0};

        // Just in-line the 'color' method here.
        unsigned long color(byte red, byte green, byte blue)
        {
            // Taken from the Adafruit_Neopixel::Color method.
            // I didn't want to add a dependency on that library for a single method, so copying it here.
            return ((unsigned long)red << 16) | ((unsigned long)green << 8) | blue;
        }
};

#endif