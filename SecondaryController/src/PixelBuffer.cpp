#include <Adafruit_NeoPixel.h>
#include <vector>
#include "Arduino.h"
#include "PixelBuffer.h"

PixelBuffer::PixelBuffer(int16_t gpioPin) {
  m_gpioPin = gpioPin;
}

void PixelBuffer::initialize(uint8_t signStyle) {
  Serial.print("Initializaing pixel buffer for style ");
  Serial.println(signStyle);
  
  switch (signStyle) {
    default:
      initializeTestMatrixBuffer(m_gpioPin);
  }

  clearBuffer();
  m_neoPixels->begin();
  m_neoPixels->clear();
}

void PixelBuffer::clearBuffer() {
  for (uint i = 0; i < m_numPixels; i++) {
    m_pixelColors[i] = 0;
  }
}

void PixelBuffer::displayPixels() {
  for (uint i = 0; i < m_numPixels; i++)
  {
    m_neoPixels->setPixelColor(i, m_pixelColors[i]);
  }

  unsigned long start = millis();
  m_neoPixels->show();

  // I have no idea why, but if we exit immediately and try to read BLE settings,
  // the BLE readings are sometimes corrupt.  If we wait until the "show" is done
  // and delay a tiny bit more, things are stable.
  while (!m_neoPixels->canShow()) {
    // wait for the "show" to complete
  }
  while (millis() - start < 10) {
    // wait until at least 10 msec have passed since starting the "show"
  }
}

unsigned int PixelBuffer::getColumnCount() {
  return m_columns.size();
}

unsigned int PixelBuffer::getRowCount() {
  return m_rows.size();
}

unsigned int PixelBuffer::getPixelCount() {
  return m_numPixels;
}

void PixelBuffer::setBrightness(uint8_t brightness) {
  m_neoPixels->setBrightness(brightness);
}

void PixelBuffer::setPixel(unsigned int pixel, uint32_t color) {
  if (pixel >= m_numPixels) {
    return;
  }

  m_pixelColors[pixel] = color;
}

void PixelBuffer::shiftLineRight(uint32_t newColor)
{
  for (uint i = m_numPixels - 1; i >= 1; i--)
  {
    m_pixelColors[i] = m_pixelColors[i - 1];
  }

  m_pixelColors[0] = newColor;
}

void PixelBuffer::shiftLineLeft(uint32_t newColor)
{
  for (uint i = 0; i < m_numPixels - 1; i++)
  {
    m_pixelColors[i] = m_pixelColors[i + 1];
  }

  m_pixelColors[m_numPixels - 1] = newColor;
}

void PixelBuffer::shiftColumnsRight(uint32_t newColor)
{
  shiftPixelBlocksRight(m_columns, newColor);
}

void PixelBuffer::shiftColumnsLeft(uint32_t newColor)
{
  shiftPixelBlocksLeft(m_columns, newColor);
}

void PixelBuffer::shiftRowsUp(uint32_t newColor)
{
  shiftPixelBlocksLeft(m_rows, newColor);
}

void PixelBuffer::shiftRowsDown(uint32_t newColor)
{
  shiftPixelBlocksRight(m_rows, newColor);
}

void PixelBuffer::shiftPixelBlocksRight(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor) {
  for (uint i = pixelBlocks.size() - 1; i >= 1; i--) {
    std::vector<int>* source = pixelBlocks.at(i - 1);
    std::vector<int>* destination = pixelBlocks.at(i);
    // Find the color of the first pixel in the source column, and set the destination column to that color.
    uint32_t previousColor = m_pixelColors[source->at(0)];
    setColorForMappedPixels(destination, previousColor);
  }

  setColorForMappedPixels(pixelBlocks.at(0), newColor);
}

void PixelBuffer::shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor) {
  for (uint i = 0; i < pixelBlocks.size() - 1; i++) {
    std::vector<int>* source = pixelBlocks.at(i + 1);
    std::vector<int>* destination = pixelBlocks.at(i);
    // Find the color of the first pixel in the source column, and set the destination column to that color.
    uint32_t previousColor = m_pixelColors[source->at(0)];
    setColorForMappedPixels(destination, previousColor);
  }

  setColorForMappedPixels(pixelBlocks.at(pixelBlocks.size() - 1), newColor);
}

void PixelBuffer::setColorForMappedPixels(std::vector<int>* destination, uint32_t newColor) {
  for (uint i = 0; i < destination->size(); i++) {
    int pixelIndex = destination->at(i);
    m_pixelColors[pixelIndex] = newColor;
  }
}

void PixelBuffer::initializeTestMatrixBuffer(int16_t gpioPin) {
  m_numPixels = 64;  // Set for the NEO PIXEL 8x8 matrix
  m_pixelColors = new uint32_t[m_numPixels];
  m_neoPixels = new Adafruit_NeoPixel(m_numPixels, gpioPin, NEO_GRB + NEO_KHZ800);

  // Map the pixel indices to rows, columns.
  // ROW 0 is at the TOP of the display.
  // COLUMN 0 is at the LEFT of the display.

  // Establish the 8 rows
  for (int col = 8; col >= 0; col--) {
    std::vector<int>* rowVector = new std::vector<int>();
    for (int row = 0; row < 8; row++) {
      if (row % 2 == 0) {
        rowVector->push_back(row * 8 + col);
      }
      else {
        rowVector->push_back(row * 8 + (7 - col));
      }
    }
    m_rows.push_back(rowVector);
  }

  // Establish the 8 cols
  // The pixels count up from the bottom, but we want rows to count up from the top,
  // so go in reverse order.
  for (int col = 7; col >= 0; col--) {
    std::vector<int>* colVector = new std::vector<int>();
    for (int row = 0; row < 8; row++){
      colVector->push_back(col * 8 + row);
    }
    m_columns.push_back(colVector);
  }
}