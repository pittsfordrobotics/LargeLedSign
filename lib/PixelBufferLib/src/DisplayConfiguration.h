#ifndef DISPLAY_CONFIGURATION_H
#define DISPLAY_CONFIGURATION_H

#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>

class DisplayConfiguration
{
    public:
        static std::vector<DisplayConfiguration*>* ParseJson(const char* jsonString);
        DisplayConfiguration(const DisplayConfiguration& other);
        
        byte getDefaultBrightness() { return defaultBrightness; }
        uint8_t getGpioPin() const { return m_gpioPin; }
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
        DisplayConfiguration();

        byte defaultBrightness{255};
        uint8_t m_gpioPin{0};
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