#ifndef NEOPIXELDISPLAY_H
#define NEOPIXELDISPLAY_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "PixelBufferLib.h"
#include "DisplayPatternLib.h"

class NeoPixelDisplay
{
    public:
        NeoPixelDisplay(const DisplayConfiguration* displayConfiguration);

        void setBrightness(byte brightess);
        void setDisplayPattern(DisplayPattern* displayPattern);
        void updateDisplay();
        void resetDisplay();

    private:
        Adafruit_NeoPixel* m_neoPixels;
        PixelBuffer2* m_pixelBuffer;
        DisplayPattern* m_displayPattern;

        void outputPixels();
};

#endif
