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

        // Sets the first pixel in the buffer to the new color,
        // shifting all the pixels in the buffer to the right by one.
        void shiftPixelsRight(uint32_t newColor);

        // Sets the last pixel in the buffer to the new color,
        // shifting all the pixels in the buffer to the left by one.
        void shiftPixelsLeft(uint32_t newColor);

        // Sets the pixels in the first column to the new color,
        // shifting all the columns to the right by one.
        void shiftColumnsRight(uint32_t newColor);

        // Sets the pixels in the starting column to the new color,
        // shifting the columns to the right by one.
        void shiftColumnsRight(uint32_t newColor, uint16_t startingColumn);

        // Sets the pixels in the last column to the new color,
        // shifting all the columns to the left by one.
        void shiftColumnsLeft(uint32_t newColor);

        // Sets the pixels in the starting column to the new color,
        // shifting the columns to the left by one.
        void shiftColumnsLeft(uint32_t newColor, uint16_t startingColumn);

        // Sets the pixels in the bottom row to the new color,
        // shifting all the rows up by one.
        void shiftRowsUp(uint32_t newColor);

        // Sets the pixels in the starting row to the new color,
        // shifting subsequent the rows up by one.
        void shiftRowsUp(uint32_t newColor, uint16_t startingRow);

        // Sets the pixels in the top row to the new color,
        // shifting all the rows down by one.
        void shiftRowsDown(uint32_t newColor);

        // Sets the pixels in the starting row to the new color,
        // shifting prior rows down by one.
        void shiftRowsDown(uint32_t newColor, uint16_t startingRow);

        // Sets the pixels in the leftmost digit to the new color,
        // shifting all the digit colors one place to the right.
        void shiftDigitsRight(uint32_t newColor);

        // Sets a random assortment of pixels in the buffer to the given color.
        void fillRandomly(uint32_t newColor, uint16_t numberOfPixels);

        // Sets the color of all pixels in the buffer to the given color.
        void fill(uint32_t newColor);

        // Gets the number of pixels in the buffer.
        uint16_t getPixelCount();

        // Gets the number of columns in the buffer.
        // Might not be needed?
        uint16_t getColumnCount();

        // Gets the number of Rows in the buffer.
        // Might not be needed?
        uint16_t getRowCount();

        uint16_t getDigitCount();

        // Set an individual pixel in the raw pixel buffer to a color.
        void setPixel(uint16_t pixel, uint32_t color);

        // Set the color of a specific pixel in the row/column map.
        void setColorInPixelMap(uint16_t row, uint16_t column, uint32_t color);

        // Set all pixels in the given row to the given color.
        void setRowColor(uint16_t row, uint32_t newColor);
        
        // Set all pixels in the given column to the given color.
        void setColumnColor(uint16_t column, uint32_t newColor);

        // Get the row-to-pixel mapping for all rows.
        const std::vector<std::vector<int>*>& getAllRows();

        // Get the column-to-pixel mapping for all columns.
        const std::vector<std::vector<int>*>& getAllColumns();

        // Get the row-to-pixel mapping for a specific digit.
        // This method calculates the intersection of rows and digits, and can be expensive to call.
        const std::vector<std::vector<int>*> getRowsForDigit(uint16_t digit);
        
        // Clears the internal pixel buffer.
        void clearBuffer();

        uint32_t getPixel(uint16_t pixel);

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
        void setColorForMappedPixels(std::vector<int>* destination, uint32_t newColor);
        void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor, uint16_t startingBlock);
        void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor, uint16_t startingBlock);
        
        // Maps a row and column to a pixel buffer index, or -1 if no pixel at that location.
        std::vector<std::vector<int>> m_pixelMap;
        // Stores the color for each row and column in the pixel map.
        std::vector<std::vector<uint32_t>> m_colorMap;
};

#endif