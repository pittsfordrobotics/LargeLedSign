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

        unsigned int getDigitsToLeft() { return m_digitsToLeft; }
        unsigned int getDigitsToRight() { return m_digitsToRight; }
        unsigned int getColsToLeft() { return m_colsToLeft; }
        unsigned int getColsToRight() { return m_colsToRight; }

        // Sets the first pixel in the buffer to the new color,
        // shifting all the pixels in the buffer to the right by one.
        void shiftPixelsRight(unsigned long newColor);

        // Sets the last pixel in the buffer to the new color,
        // shifting all the pixels in the buffer to the left by one.
        void shiftPixelsLeft(unsigned long newColor);

        // Sets the pixels in the first column to the new color,
        // shifting all the columns to the right by one.
        void shiftColumnsRight(unsigned long newColor);

        // Sets the pixels in the starting column to the new color,
        // shifting the columns to the right by one.
        void shiftColumnsRight(unsigned long newColor, unsigned int startingColumn);

        // Sets the pixels in the last column to the new color,
        // shifting all the columns to the left by one.
        void shiftColumnsLeft(unsigned long newColor);

        // Sets the pixels in the starting column to the new color,
        // shifting the columns to the left by one.
        void shiftColumnsLeft(unsigned long newColor, unsigned int startingColumn);

        // Sets the pixels in the bottom row to the new color,
        // shifting all the rows up by one.
        void shiftRowsUp(unsigned long newColor);

        // Sets the pixels in the starting row to the new color,
        // shifting subsequent the rows up by one.
        void shiftRowsUp(unsigned long newColor, unsigned int startingRow);

        // Sets the pixels in the top row to the new color,
        // shifting all the rows down by one.
        void shiftRowsDown(unsigned long newColor);

        // Sets the pixels in the starting row to the new color,
        // shifting prior rows down by one.
        void shiftRowsDown(unsigned long newColor, unsigned int startingRow);

        // Sets the pixels in the leftmost digit to the new color,
        // shifting all the digit colors one place to the right.
        void shiftDigitsRight(unsigned long newColor);

        // Sets a random assortment of pixels in the buffer to the given color.
        void fillRandomly(unsigned long newColor, unsigned int numberOfPixels);

        // Sets the color of all pixels in the buffer to the given color.
        void fill(unsigned long newColor);

        // Gets the number of pixels in the buffer.
        unsigned int getPixelCount();

        // Gets the number of columns in the buffer.
        // Might not be needed?
        unsigned int getColumnCount();

        // Gets the number of Rows in the buffer.
        // Might not be needed?
        unsigned int getRowCount();

        unsigned int getDigitCount();

        // Set an individual pixel in the raw pixel buffer to a color.
        void setPixel(unsigned int pixel, unsigned long color);

        // Set the color of a specific pixel in the row/column map.
        void setColorInPixelMap(unsigned int row, unsigned int column, unsigned long color);

        // Set all pixels in the given row to the given color.
        void setRowColor(unsigned int row, unsigned long newColor);
        
        // Set all pixels in the given column to the given color.
        void setColumnColor(unsigned int column, unsigned long newColor);

        // Get the row-to-pixel mapping for all rows.
        const std::vector<std::vector<int>*>& getAllRows();

        // Get the column-to-pixel mapping for all columns.
        const std::vector<std::vector<int>*>& getAllColumns();

        // Get the row-to-pixel mapping for a specific digit.
        // This method calculates the intersection of rows and digits, and can be expensive to call.
        const std::vector<std::vector<int>*> getRowsForDigit(unsigned int digit);
        
        // Clears the internal pixel buffer.
        void clearBuffer();

        unsigned long getPixel(unsigned int pixel);

    private:
        unsigned int m_numPixels{0};       // Size of internal buffer
        unsigned long* m_pixelColors;      // Underlying pixels buffer used to output to LEDs
        unsigned int m_digitsToLeft{0};
        unsigned int m_digitsToRight{0};
        unsigned int m_colsToLeft{0};
        unsigned int m_colsToRight{0};
        std::vector<std::vector<int>*> m_columns;
        std::vector<std::vector<int>*> m_rows;
        std::vector<std::vector<int>*> m_digits;
        
        void initializeFromConfiguration(const DisplayConfiguration& displayConfiguration);
        void initializeInternalMaps();
        void setRowAndColumnForPixel(uint pixel, uint row, uint column);
        void setColorForMappedPixels(std::vector<int>* destination, unsigned long newColor);
        void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, unsigned long newColor, unsigned int startingBlock);
        void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, unsigned long newColor, unsigned int startingBlock);
        
        // Maps a row and column to a pixel buffer index, or -1 if no pixel at that location.
        std::vector<std::vector<int>> m_pixelMap;
        // Stores the color for each row and column in the pixel map.
        std::vector<std::vector<unsigned long>> m_colorMap;
};

#endif