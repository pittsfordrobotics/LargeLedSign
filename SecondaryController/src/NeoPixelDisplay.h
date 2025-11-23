/*
Duplicate of the code under the LecgacyController folder.
The Secondary and Legacy folder should eventually be merged.
*/

#ifndef NEOPIXEL_DISPLAY_H
#define NEOPIXEL_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <PixelMap.h>
#include <DisplayConfiguration.h>
#include <DisplayPattern.h>

class NeoPixelDisplay
{
    public:
        NeoPixelDisplay(const DisplayConfiguration& displayConfiguration);

        void setBrightness(byte brightess);
        void setDisplayPattern(DisplayPattern* displayPattern);
        DisplayPattern* getDisplayPattern() { return m_displayPattern; }
        void updateDisplay();
        void resetDisplay();

        // Temp
        PixelMap* getPixelMap() { return m_pixelMap; }

    private:
        Adafruit_NeoPixel* m_neoPixels;
        PixelMap* m_pixelMap;
        DisplayPattern* m_displayPattern;

        void outputPixels();
};

#endif
