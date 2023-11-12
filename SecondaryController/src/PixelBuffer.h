#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <vector>

#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

class PixelBuffer {
  public:
    PixelBuffer(int16_t gpioPin);

    void initialize(uint8_t signStyle);

    void setColsToLeft(uint16_t colsToLeft) { m_colsToLeft = colsToLeft; }
    uint16_t getColsToLeft() { return m_colsToLeft; }

    void setColsToRight(uint16_t colsToRight) { m_colsToRight = colsToRight; }
    uint16_t getColsToRight() { return m_colsToRight; }

    void setBrightness(uint8_t brightess);

    // Sets the first pixel in the buffer to the new color,
    // shifting all the pixels in the buffer to the right by one.
    void shiftLineRight(uint32_t newColor);

    // Sets the last pixel in the buffer to the new color,
    // shifting all the pixels in the buffer to the left by one.
    void shiftLineLeft(uint32_t newColor);

    // Sets the pixels in the first column to the new color,
    // shifting all the columns to the right by one.
    void shiftColumnsRight(uint32_t newColor);

    // Sets the pixels in the last column to the new color,
    // shifting all the columns to the left by one.
    void shiftColumnsLeft(uint32_t newColor);

    // Sets the pixels in the bottom row to the new color,
    // shifting all the rows up by one.
    void shiftRowsUp(uint32_t newColor);

    // Sets the pixels in the top row to the new color,
    // shifting all the rows down by one.
    void shiftRowsDown(uint32_t newColor);

    // Gets the number of pixels in the buffer.
    unsigned int getPixelCount();

    // Gets the number of columns in the buffer.
    // Might not be needed?
    unsigned int getColumnCount();

    // Gets the number of Rows in the buffer.
    // Might not be needed?
    unsigned int getRowCount();

    // Set an individual pixel in the buffer to a color.
    void setPixel(unsigned int pixel, uint32_t color);

    // Output the interal pixel buffer to the NeoPixel LEDs.
    void displayPixels();

    // Clears the internal pixel buffer, but does not reset the NeoPixel LEDs.
    void clearBuffer();

  private:
    int m_gpioPin;
    Adafruit_NeoPixel* m_neoPixels;
    unsigned int m_numPixels;
    uint32_t* m_pixelColors;
    uint16_t m_colsToLeft;
    uint16_t m_colsToRight;
    std::vector<std::vector<int>*> m_columns;
    std::vector<std::vector<int>*> m_rows;
    
    void initializeTestRingBuffer(int16_t gpioPin);
    void initializeTestMatrixBuffer(int16_t gpioPin, uint8_t signStyle);
    void setColorForMappedPixels(std::vector<int>* destination, uint32_t newColor);
    void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor);
    void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor);
};

#endif