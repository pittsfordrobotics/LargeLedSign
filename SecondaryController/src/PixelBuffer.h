#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <algorithm>

#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

class PixelBuffer {
  public:
    PixelBuffer(int gpioPin);

    void initialize(byte signStyle);

    void setDigitsToLeft(uint digitsToLeft) { m_digitsToLeft = digitsToLeft; }
    void setDigitsToRight(uint digitsToRight) { m_digitsToRight = digitsToRight; }
    void setColsToLeft(uint colsToLeft) { m_colsToLeft = colsToLeft; }
    void setColsToRight(uint colsToRight) { m_colsToRight = colsToRight; }

    uint getDigitsToLeft() { return m_digitsToLeft; }
    uint getDigitsToRight() { return m_digitsToRight; }
    uint getColsToLeft() { return m_colsToLeft; }
    uint getColsToRight() { return m_colsToRight; }

    void setBrightness(byte brightess);

    // Sets the first pixel in the buffer to the new color,
    // shifting all the pixels in the buffer to the right by one.
    void shiftPixelsRight(ulong newColor);

    // Sets the last pixel in the buffer to the new color,
    // shifting all the pixels in the buffer to the left by one.
    void shiftPixelsLeft(ulong newColor);

    // Sets the pixels in the first column to the new color,
    // shifting all the columns to the right by one.
    void shiftColumnsRight(ulong newColor);

    // Sets the pixels in the starting column to the new color,
    // shifting the columns to the right by one.
    void shiftColumnsRight(ulong newColor, uint startingColumn);

    // Sets the pixels in the last column to the new color,
    // shifting all the columns to the left by one.
    void shiftColumnsLeft(ulong newColor);

    // Sets the pixels in the starting column to the new color,
    // shifting the columns to the left by one.
    void shiftColumnsLeft(ulong newColor, uint startingColumn);

    // Sets the pixels in the bottom row to the new color,
    // shifting all the rows up by one.
    void shiftRowsUp(ulong newColor);

    // Sets the pixels in the starting row to the new color,
    // shifting subsequent the rows up by one.
    void shiftRowsUp(ulong newColor, uint startingRow);

    // Sets the pixels in the top row to the new color,
    // shifting all the rows down by one.
    void shiftRowsDown(ulong newColor);

    // Sets the pixels in the starting row to the new color,
    // shifting prior rows down by one.
    void shiftRowsDown(ulong newColor, uint startingRow);

    // Sets a random assortment of pixels in the buffer to the given color.
    void fillRandomly(ulong newColor, uint numberOfPixels);

    // Sets the color of all pixels in the buffer to the given color.
    void fill(ulong newColor);

    // Gets the number of pixels in the buffer.
    uint getPixelCount();

    // Gets the number of columns in the buffer.
    // Might not be needed?
    uint getColumnCount();

    // Gets the number of Rows in the buffer.
    // Might not be needed?
    uint getRowCount();

    // Set an individual pixel in the buffer to a color.
    void setPixel(uint pixel, ulong color);

    // Output the internal pixel buffer to the NeoPixel LEDs.
    void displayPixels();

    // Clears the internal pixel buffer, but does not reset the NeoPixel LEDs.
    void clearBuffer();

  private:
    int m_gpioPin;
    Adafruit_NeoPixel* m_neoPixels;
    uint m_numPixels{0};
    ulong* m_pixelColors;
    uint m_digitsToLeft{0};
    uint m_digitsToRight{0};
    uint m_colsToLeft{0};
    uint m_colsToRight{0};
    std::vector<std::vector<int>*> m_columns;
    std::vector<std::vector<int>*> m_rows;
    
    void setColorForMappedPixels(std::vector<int>* destination, ulong newColor);
    void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, ulong newColor, uint startingBlock);
    void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, ulong newColor, uint startingBlock);
    void initializeTestMatrix();
    void initializeDigitOne();
    void initializeDigitThree();
    void initializeDigitEight();
    void initializeLogo();
};

#endif