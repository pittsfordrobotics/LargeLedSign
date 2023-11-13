#include <Adafruit_NeoPixel.h>
#include <vector>
#include "Arduino.h"
#include "PixelBuffer.h"

PixelBuffer::PixelBuffer(int16_t gpioPin) {
  m_gpioPin = gpioPin;
}

void PixelBuffer::initialize(uint8_t signStyle) {
  //initializeTestRingBuffer(gpioPin);
  initializeTestMatrixBuffer(m_gpioPin, signStyle);

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
  for (int i = m_numPixels - 1; i >= 1; i--)
  {
    m_pixelColors[i] = m_pixelColors[i - 1];
  }

  m_pixelColors[0] = newColor;
}

void PixelBuffer::shiftLineLeft(uint32_t newColor)
{
  for (int i = 0; i < m_numPixels - 1; i++)
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
  for (int i = pixelBlocks.size() - 1; i >= 1; i--) {
    std::vector<int>* source = pixelBlocks.at(i - 1);
    std::vector<int>* destination = pixelBlocks.at(i);
    // Find the color of the first pixel in the source column, and set the destination column to that color.
    uint32_t previousColor = m_pixelColors[source->at(0)];
    setColorForMappedPixels(destination, previousColor);
  }

  setColorForMappedPixels(pixelBlocks.at(0), newColor);
}

void PixelBuffer::shiftPixelBlocksLeft(std::vector<std::vector<int>*> pixelBlocks, uint32_t newColor) {
  for (int i = 0; i < pixelBlocks.size() - 1; i++) {
    std::vector<int>* source = pixelBlocks.at(i + 1);
    std::vector<int>* destination = pixelBlocks.at(i);
    // Find the color of the first pixel in the source column, and set the destination column to that color.
    uint32_t previousColor = m_pixelColors[source->at(0)];
    setColorForMappedPixels(destination, previousColor);
  }

  setColorForMappedPixels(pixelBlocks.at(pixelBlocks.size() - 1), newColor);
}

void PixelBuffer::setColorForMappedPixels(std::vector<int>* destination, uint32_t newColor) {
  for (int i = 0; i < destination->size(); i++) {
    int pixelIndex = destination->at(i);
    m_pixelColors[pixelIndex] = newColor;
  }
}

void PixelBuffer::initializeTestRingBuffer(int16_t gpioPin) {
  m_numPixels = 12;  // Set for the NEO PIXEL 12-LED ring for testing
  m_pixelColors = new uint32_t[m_numPixels];
  m_neoPixels = new Adafruit_NeoPixel(m_numPixels, gpioPin, NEO_GRB + NEO_KHZ800);

  // Map the pixel indices to rows, columns.
  // ROW 0 is at the TOP of the display.
  // COLUMN 0 is at the LEFT of the display.

  // Set up the rows/columns as small pieces of the NeoPixel ring
  m_columns.push_back(new std::vector<int>{0});
  m_columns.push_back(new std::vector<int>{1, 11});
  m_columns.push_back(new std::vector<int>{2, 10});
  m_columns.push_back(new std::vector<int>{3, 9});
  m_columns.push_back(new std::vector<int>{4, 8});
  m_columns.push_back(new std::vector<int>{5, 7});
  m_columns.push_back(new std::vector<int>{6});

  m_rows.push_back(new std::vector<int>{3});
  m_rows.push_back(new std::vector<int>{2, 4});
  m_rows.push_back(new std::vector<int>{1, 5});
  m_rows.push_back(new std::vector<int>{0, 6});
  m_rows.push_back(new std::vector<int>{11, 7});
  m_rows.push_back(new std::vector<int>{10, 8});
  m_rows.push_back(new std::vector<int>{9});
}

void PixelBuffer::initializeTestMatrixBuffer(int16_t gpioPin, uint8_t signStyle) {
  // signStyle = 0 will be the "first" in the sequence
  //           = 1 will be the "second" in the sequence
  m_numPixels = 64;  // Set for the NEO PIXEL 8x8 matrix

  // TESTING
  // if (signStyle == 1) {
  //   // For sign "1", let it think it has 1000 pixels.
  //   m_numPixels = 1000;
  // }
  m_pixelColors = new uint32_t[m_numPixels];
  m_neoPixels = new Adafruit_NeoPixel(m_numPixels, gpioPin, NEO_GRB + NEO_KHZ800);

  // Map the pixel indices to rows, columns.
  // ROW 0 is at the TOP of the display.
  // COLUMN 0 is at the LEFT of the display.

  // Establish the 8 columns
  for (int col = 0; col < 8; col++) {
    std::vector<int>* colVector = new std::vector<int>();
    for (int row = 0; row < 8; row++) {
      if (row % 2 == 0) {
        colVector->push_back(row * 8 + col);
      }
      else {
        colVector->push_back(row * 8 + (7 - col));
      }
    }
    m_columns.push_back(colVector);
  }

  // Establish the 8 rows
  // The pixes count up from the bottom, but we want rows to count up from the top,
  // so go in reverse order.
  for (int row = 7; row >= 0; row--) {
    std::vector<int>* rowVector = new std::vector<int>();
    for (int col = 0; col < 8; col++){
      rowVector->push_back(row * 8 + col);
    }
    m_rows.push_back(rowVector);
  }
}