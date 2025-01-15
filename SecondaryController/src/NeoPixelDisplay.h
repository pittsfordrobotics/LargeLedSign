#ifndef NEOPIXELDISPLAY_H
#define NEOPIXELDISPLAY_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "PixelBufferLib.h"
#include "DisplayPatternLib.h"

class NeoPixelDisplay
{
    public:
        NeoPixelDisplay(const DisplayConfiguration& displayConfiguration);

        void setBrightness(byte brightess);
        void setDisplayPattern(DisplayPattern* displayPattern) { m_displayPattern = displayPattern; }
        DisplayPattern* getDisplayPattern() { return m_displayPattern; }
        void updateDisplay();
        void resetDisplay();

        // Temp
        PixelBuffer* getPixelBuffer() { return m_pixelBuffer; }

    private:
        Adafruit_NeoPixel* m_neoPixels;
        PixelBuffer* m_pixelBuffer;
        DisplayPattern* m_displayPattern;

        void outputPixels();
};

#endif
