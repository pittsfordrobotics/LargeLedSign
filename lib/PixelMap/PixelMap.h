#ifndef PIXEL_MAP_H
#define PIXEL_MAP_H

#include <Arduino.h>
#include <vector>
#include <algorithm>
#include <DisplayConfiguration.h>

class PixelMap
{
    public:
        PixelMap(const DisplayConfiguration& displayConfiguration);
        ~PixelMap();

        uint16_t getDigitsToLeft() { return m_digitsToLeft; }
        uint16_t getDigitsToRight() { return m_digitsToRight; }
        uint16_t getColsToLeft() { return m_colsToLeft; }
        uint16_t getColsToRight() { return m_colsToRight; }

        void setDigitsToLeft(uint16_t digitsToLeft) { m_digitsToLeft = digitsToLeft; }
        void setDigitsToRight(uint16_t digitsToRight) { m_digitsToRight = digitsToRight; }
        void setColsToLeft(uint16_t colsToLeft) { m_colsToLeft = colsToLeft; }
        void setColsToRight(uint16_t colsToRight) { m_colsToRight = colsToRight; }

        void shiftPixelsRight();
        void shiftPixelsLeft();
        void shiftColumnsRight();
        void shiftColumnsRight(uint16_t startingColumn);
        void shiftColumnsLeft();
        void shiftColumnsLeft(uint16_t startingColumn);
        void shiftRowsUp();
        void shiftRowsUp(uint16_t startingRow);
        void shiftRowsDown();
        void shiftRowsDown(uint16_t startingRow);
        void shiftDigitsRight();
        void shiftDigitsLeft();

        uint16_t getPixelCount();
        uint16_t getColumnCount();
        uint16_t getRowCount();
        uint16_t getDigitCount();

        // Sets a random assortment of pixels in the buffer to the given color.
        void fillRandomly(uint32_t newColor, uint16_t numberOfPixels);

        // Sets the color of all pixels in the buffer to the given color.
        void fill(uint32_t newColor);

        // Set an individual pixel in the raw pixel buffer to a color.
        void setRawPixelColor(uint16_t pixel, uint32_t color);

        // Set the color of a specific pixel in the row/column map.
        void setColorInPixelMap(uint16_t row, uint16_t column, uint32_t color);

        void setRowColor(uint16_t row, uint32_t newColor);
        void setColumnColor(uint16_t column, uint32_t newColor);
        void setDigitColor(uint16_t digit, uint32_t newColor);

        // Get the row-to-pixel mapping for all rows.
        const std::vector<std::vector<int>*>& getAllRows();

        // Get the column-to-pixel mapping for all columns.
        const std::vector<std::vector<int>*>& getAllColumns();

        // Get the row-to-pixel mapping for a specific digit.
        // This method calculates the intersection of rows and digits, and can be expensive to call.
        // (Used only by the Fire pattern, and only for a style that's not often used.)
        const std::vector<std::vector<int>*> getRowsForDigit(uint16_t digit);
        
        // Clears the internal pixel buffer.
        void clearBuffer();

        uint32_t getRawPixelColor(uint16_t pixel);

    private:
        uint16_t m_numPixels{0};       // Size of internal buffer
        uint32_t* m_pixelColors;      // Underlying pixels buffer used to output to LEDs
        uint16_t m_digitsToLeft{0};
        uint16_t m_digitsToRight{0};
        uint16_t m_colsToLeft{0};
        uint16_t m_colsToRight{0};
        std::vector<std::vector<int>*> m_columns;
        std::vector<std::vector<int>*> m_rows;
        std::vector<std::vector<int>*> m_digits;
        
        void initializeFromConfiguration(const DisplayConfiguration& displayConfiguration);
        void initializeInternalMaps();
        void setRowAndColumnForPixel(uint16_t pixel, uint16_t row, uint16_t column);
        
        // Maps a row and column to a pixel buffer index, or -1 if no pixel at that location.
        std::vector<std::vector<int>> m_pixelMap;
        // Stores the color for each row and column in the pixel map.
        std::vector<std::vector<uint32_t>> m_colorMap;
};

#endif