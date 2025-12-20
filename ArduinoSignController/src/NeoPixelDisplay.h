#ifndef NEOPIXEL_DISPLAY_H
#define NEOPIXEL_DISPLAY_H

#include <Arduino.h>
#include <pico/mutex.h>
#include <Adafruit_NeoPixel.h>
#include "PixelMap\PixelMap.h"
#include "Configuration\DisplayConfiguration.h"
#include "Patterns\DisplayPatterns\DisplayPattern.h"

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
        uint16_t getPixelCount();
        uint16_t getColumnCount();
        uint16_t getRowCount();
        uint16_t getDigitCount();

        void setDigitsToLeft(uint16_t digitsToLeft);
        void setDigitsToRight(uint16_t digitsToRight);
        void setColumnsToLeft(uint16_t colsToLeft);
        void setColumnsToRight(uint16_t colsToRight);

    private:
        mutex_t m_lockObject;  // Pico SDK mutex for locking when dealing with display pattern changes.
        Adafruit_NeoPixel* m_neoPixels;
        PixelMap* m_pixelMap;
        DisplayPattern* m_displayPattern;

        void outputPixels();
};

#endif
