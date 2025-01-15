#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <Arduino.h>
#include <vector>
#include <algorithm>
#include "DisplayConfiguration.h"

class PixelBuffer {
  public:
    PixelBuffer(const DisplayConfiguration& displayConfiguration);

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

    // Sets the first pixel in the line to the new color,
    // shifting all other pixel colors one place further down the line.
    void shiftLine(unsigned long newColor);

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

    // Set an individual pixel in the buffer to a color.
    void setPixel(unsigned int pixel, unsigned long color);

    unsigned long getPixel(unsigned int pixel);

    // Clears the internal pixel buffer.
    void clearBuffer();
    
  private:
    unsigned int m_numPixels{0};
    unsigned long* m_pixelBuffer;;
    unsigned int m_digitsToLeft{0};
    unsigned int m_digitsToRight{0};
    unsigned int m_colsToLeft{0};
    unsigned int m_colsToRight{0};
    std::vector<std::vector<int>*> m_columns;
    std::vector<std::vector<int>*> m_rows;
    std::vector<std::vector<int>*> m_digits;

    void setColorForMappedPixels(std::vector<int>* destination, unsigned long newColor);
    void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, unsigned long newColor, unsigned int startingBlock);
    void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, unsigned long newColor, unsigned int startingBlock);
};

#endif