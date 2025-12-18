#ifndef DISPLAY_CONFIGURATION_H
#define DISPLAY_CONFIGURATION_H

#ifdef PIO_UNIT_TESTING
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include <vector>
#include <ArduinoJson.h>
#include <DebugUtils.h>

#define DISPLAY_CONFIG_DEFAULTBRIGHTNESS 170

class DisplayConfiguration
{
    public:
        static std::vector<DisplayConfiguration>* ParseJson(const char* jsonString, size_t length);
        DisplayConfiguration(const DisplayConfiguration& other);
        ~DisplayConfiguration();
        
        byte getDefaultBrightness() const { return m_defaultBrightness; }
        byte getGpioPin() const { return m_gpioPin; }
        uint16_t getNumberOfPixels() { return m_numPixels; }
        uint16_t getNumberOfRows() { return m_numRows; }
        uint16_t getNumberOfColumns() { return m_numCols; }
        uint16_t getColumnsToLeft() { return m_colsToLeft; }
        uint16_t getColumnsToRight() { return m_colsToRight; }
        uint16_t getRowsAbove() { return m_rowsAbove; }
        uint16_t getRowsBelow() { return m_rowsBelow; }
        uint16_t getDigitsToLeft() { return m_digitsToLeft; }
        uint16_t getDigitsToRight() { return m_digitsToRight; }
        std::vector<std::vector<uint16_t>*> getColumnPixelMapping() { return m_columnPixelMapping; }
        std::vector<std::vector<uint16_t>*> getRowPixelMapping() { return m_rowPixelMapping; }
        std::vector<std::vector<uint16_t>*> getDigitPixelMapping() { return m_digitPixelMapping; }
        
        DisplayConfiguration& operator=(const DisplayConfiguration& other);

    private:
        static bool tryParseDisplayEntryFromJsonVariant(JsonVariant display, byte defaultBrightness, DisplayConfiguration& config);

        DisplayConfiguration();

        byte m_defaultBrightness{255};
        byte m_gpioPin{0};
        uint16_t m_numPixels{0};
        uint16_t m_numCols{0};
        uint16_t m_numRows{0};
        uint16_t m_colsToLeft{0};
        uint16_t m_colsToRight{0};
        uint16_t m_rowsAbove{0};
        uint16_t m_rowsBelow{0};
        uint16_t m_digitsToLeft{0};
        uint16_t m_digitsToRight{0};
        std::vector<std::vector<uint16_t>*> m_columnPixelMapping;
        std::vector<std::vector<uint16_t>*> m_rowPixelMapping;
        std::vector<std::vector<uint16_t>*> m_digitPixelMapping;

        void copy(const DisplayConfiguration& other);
};
#endif // DISPLAY_CONFIGURATION_H