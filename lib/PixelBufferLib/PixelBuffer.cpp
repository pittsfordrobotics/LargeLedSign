#include <Adafruit_NeoPixel.h>
#include <vector>
#include "Arduino.h"
#include "PixelBuffer.h"

// All pixel buffers will have at least this many pixels, even if not used.
// This is done to help the signs stay in sync.
uint minimumPixelsInBuffer = 360;

PixelBuffer::PixelBuffer(int gpioPin)
{
    m_gpioPin = gpioPin;
}

void PixelBuffer::initialize()
{
    m_neoPixels = new Adafruit_NeoPixel(m_pixelBufferSize, m_gpioPin, NEO_GRB + NEO_KHZ800);
    clearBuffer();
    m_neoPixels->begin();
    m_neoPixels->clear();
}

void PixelBuffer::clearBuffer()
{
    for (uint i = 0; i < m_pixelBufferSize; i++)
    {
        m_pixelColors[i] = 0;
    }
}

void PixelBuffer::stop()
{
    m_isStopped = true;
}

void PixelBuffer::resume()
{
    m_isStopped = false;
}

void PixelBuffer::displayPixels()
{
    if (m_isStopped)
    {
        return;
    }

    for (uint i = 0; i < m_pixelBufferSize; i++)
    {
        m_neoPixels->setPixelColor(i, m_pixelColors[i]);
    }

    unsigned long start = millis();
    m_neoPixels->show();

    // I have no idea why, but if we exit immediately and try to read BLE settings,
    // the BLE readings are sometimes corrupt.  If we wait until the "show" is done
    // and delay a tiny bit more, things are stable.
    while (!m_neoPixels->canShow())
    {
        // wait for the "show" to complete
    }
    while (millis() - start < 10)
    {
        // wait until at least 10 msec have passed since starting the "show"
    }
}

uint PixelBuffer::getColumnCount()
{
    return m_columns.size();
}

uint PixelBuffer::getRowCount()
{
    return m_rows.size();
}

uint PixelBuffer::getPixelCount()
{
    return m_numPixels;
}

void PixelBuffer::setBrightness(byte brightness)
{
    m_neoPixels->setBrightness(brightness);
}

void PixelBuffer::setPixel(unsigned int pixel, ulong color)
{
    if (pixel >= m_pixelBufferSize)
    {
        return;
    }

    m_pixelColors[pixel] = color;
}

void PixelBuffer::fill(ulong newColor)
{
    for (uint i = 0; i < m_pixelBufferSize; i++)
    {
        m_pixelColors[i] = newColor;
    }
}

void PixelBuffer::fillRandomly(ulong newColor, uint numberOfPixels)
{
    for (uint i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelColors[pixel] = newColor;
    }
}

void PixelBuffer::shiftPixelsRight(ulong newColor)
{
    for (uint i = m_pixelBufferSize - 1; i >= 1; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelBuffer::shiftPixelsLeft(ulong newColor)
{
    for (uint i = 0; i < m_pixelBufferSize - 1; i++)
    {
        m_pixelColors[i] = m_pixelColors[i + 1];
    }

    m_pixelColors[m_numPixels - 1] = newColor;
}

void PixelBuffer::shiftColumnsRight(ulong newColor)
{
    shiftPixelBlocksRight(m_columns, newColor, 0);
}

void PixelBuffer::shiftColumnsRight(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftColumnsLeft(ulong newColor)
{
    shiftPixelBlocksLeft(m_columns, newColor, m_columns.size() - 1);
}

void PixelBuffer::shiftColumnsLeft(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftRowsUp(ulong newColor)
{
    shiftPixelBlocksLeft(m_rows, newColor, m_rows.size() - 1);
}

void PixelBuffer::shiftRowsUp(ulong newColor, uint startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftRowsDown(ulong newColor)
{
    shiftPixelBlocksRight(m_rows, newColor, 0);
}

void PixelBuffer::shiftRowsDown(ulong newColor, uint startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftDigitsRight(ulong newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
}

void PixelBuffer::shiftLine(ulong newColor)
{
    for (uint i = m_pixelBufferSize - 1; i > 0; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelBuffer::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, ulong newColor, uint startingBlock)
{
    for (uint i = pixelBlocks.size() - 1; i > startingBlock; i--)
    {
        std::vector<int> *source = pixelBlocks.at(i - 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, ulong newColor, uint startingBlock)
{
    for (uint i = 0; i < startingBlock; i++)
    {
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer::setColorForMappedPixels(std::vector<int> *destination, uint32_t newColor)
{
    for (uint i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        m_pixelColors[pixelIndex] = newColor;
    }
}