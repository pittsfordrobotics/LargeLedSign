#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <algorithm>

#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

class PixelBuffer {
  public:
    PixelBuffer(int16_t gpioPin);

    void initialize(uint8_t signStyle);

    void setDigitsToLeft(uint16_t digitsToLeft) { m_digitsToLeft = digitsToLeft; }
    void setDigitsToRight(uint16_t digitsToRight) { m_digitsToRight = digitsToRight; }
    void setColsToLeft(uint16_t colsToLeft) { m_colsToLeft = colsToLeft; }
    void setColsToRight(uint16_t colsToRight) { m_colsToRight = colsToRight; }
    void setPixelsToLeft(uint16_t pixelsToLeft) { m_pixelsToLeft = pixelsToLeft; }
    void setPixelsToRight(uint16_t pixelsToRight) { m_pixelsToRight = pixelsToRight; }

    u_int16_t getDigitsToLeft() { return m_digitsToLeft; }
    u_int16_t getDigitsToRight() { return m_digitsToRight; }
    u_int16_t getColsToLeft() { return m_colsToLeft; }
    u_int16_t getColsToRight() { return m_colsToRight; }
    u_int16_t getPixelsToLeft() { return m_pixelsToLeft; }
    u_int16_t getPixelsToRight() { return m_pixelsToRight; }

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
    unsigned int m_numPixels{0};
    uint32_t* m_pixelColors;
    uint16_t m_digitsToLeft{0};
    uint16_t m_digitsToRight{0};
    uint16_t m_colsToLeft{0};
    uint16_t m_colsToRight{0};
    uint16_t m_pixelsToLeft{0};
    uint16_t m_pixelsToRight{0};
    std::vector<std::vector<int>*> m_columns;
    std::vector<std::vector<int>*> m_rows;
    
    void initializeTestMatrixBuffer();
    void setColorForMappedPixels(std::vector<int>* destination, uint32_t newColor);
    void shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor);
    void shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor);
};

#endif