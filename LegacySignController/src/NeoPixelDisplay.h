#ifndef NEOPIXEL_DISPLAY_H
#define NEOPIXEL_DISPLAY_H

#include <Arduino.h>
#include <pico/mutex.h>
#include <Adafruit_NeoPixel.h>
#include <PixelMap.h>
#include <DisplayConfiguration.h>
#include <DisplayPattern.h>

class NeoPixelDisplay
{
    public:
        NeoPixelDisplay(const DisplayConfiguration& displayConfiguration);

        void setBrightness(byte brightess);
        byte getBrightness();
        void setDisplayPattern(DisplayPattern* displayPattern);
        void setSpeed(byte speed);
        DisplayPattern* getDisplayPattern() { return m_displayPattern; }
        void updateDisplay();
        void resetDisplay();

        // Temp
        PixelMap* getPixelMap() { return m_pixelMap; }

    private:
        mutex_t m_lockObject;  // Pico SDK mutex for locking when dealing with display pattern changes.
        Adafruit_NeoPixel* m_neoPixels;
        PixelMap* m_pixelMap;
        DisplayPattern* m_displayPattern;

        void outputPixels();
};

#endif
