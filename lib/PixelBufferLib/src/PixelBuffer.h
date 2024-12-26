#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <algorithm>

#define PB_MINIMUM_PIXELS 360

class PixelBuffer {
  public:
    PixelBuffer(int gpioPin);
    static PixelBuffer* FromJson(String jsonString);

    void initialize();

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

    // Sets the pixels in the leftmost digit to the new color,
    // shifting all the digit colors one place to the right.
    void shiftDigitsRight(ulong newColor);

    // Sets the first pixel in the line to the new color,
    // shifting all other pixel colors one place further down the line.
    void shiftLine(ulong newColor);

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

    // Stop processing any LED output updates.
    void stop();

    // Resume processing LED output updates.
    void resume();

  protected:
    uint m_numPixels{0};       // Size of internal buffer
    uint m_pixelBufferSize{0}; // Size of NeoPixel buffer
    ulong* m_pixelColors;
    uint m_digitsToLeft{0};
    uint m_digitsToRight{0};
    uint m_colsToLeft{0};
    uint m_colsToRight{0};
    std::vector<std::vector<int>*> m_columns;
    std::vector<std::vector<int>*> m_rows;
    std::vector<std::vector<int>*> m_digits;
    
  private:
    bool m_isStopped{false};
    int m_gpioPin;
    Adafruit_NeoPixel* m_neoPixels;
    void setColorForMappedPixels(std::vector<int>* destination, ulong newColor);
    void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, ulong newColor, uint startingBlock);
    void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, ulong newColor, uint startingBlock);
};

#endif