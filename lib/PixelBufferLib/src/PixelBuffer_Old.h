#ifndef PIXEL_BUFFER_OLD_H
#define PIXEL_BUFFER_OLD_H

#include <Arduino.h>
#include <vector>
#include <algorithm>
#include "DisplayConfiguration.h"

#define PB_MINIMUM_PIXELS 360

class PixelBuffer_Old {
  public:
    PixelBuffer_Old(int gpioPin);
    PixelBuffer_Old(const DisplayConfiguration* displayConfiguration);

    void initialize();

    void setDigitsToLeft(unsigned int digitsToLeft) { m_digitsToLeft = digitsToLeft; }
    void setDigitsToRight(unsigned int digitsToRight) { m_digitsToRight = digitsToRight; }
    void setColsToLeft(unsigned int colsToLeft) { m_colsToLeft = colsToLeft; }
    void setColsToRight(unsigned int colsToRight) { m_colsToRight = colsToRight; }

    unsigned int getDigitsToLeft() { return m_digitsToLeft; }
    unsigned int getDigitsToRight() { return m_digitsToRight; }
    unsigned int getColsToLeft() { return m_colsToLeft; }
    unsigned int getColsToRight() { return m_colsToRight; }

    void setBrightness(byte brightess);

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

    // Output the internal pixel buffer to the NeoPixel LEDs.
    void displayPixels();

    // Clears the internal pixel buffer, but does not reset the NeoPixel LEDs.
    void clearBuffer();

    // Stop processing any LED output updates.
    void stop();

    // Resume processing LED output updates.
    void resume();

  protected:
    unsigned int m_numPixels{0};       // Size of internal buffer
    unsigned int m_pixelBufferSize{0}; // Size of NeoPixel buffer
    unsigned long* m_pixelColors;
    unsigned int m_digitsToLeft{0};
    unsigned int m_digitsToRight{0};
    unsigned int m_colsToLeft{0};
    unsigned int m_colsToRight{0};
    std::vector<std::vector<int>*> m_columns;
    std::vector<std::vector<int>*> m_rows;
    std::vector<std::vector<int>*> m_digits;
    
  private:
    bool m_isStopped{false};
    int m_gpioPin;
    void setColorForMappedPixels(std::vector<int>* destination, unsigned long newColor);
    void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, unsigned long newColor, unsigned int startingBlock);
    void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, unsigned long newColor, unsigned int startingBlock);
};

#endif