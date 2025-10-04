#include <Adafruit_NeoPixel.h>
#include <vector>
#include "Arduino.h"
#include "PixelBuffer.h"

PixelBuffer::PixelBuffer(int gpioPin)
{
    // Todo: numPixels should be passed in here instead of set later.
    // m_pixelColors should be set here based on numPixels.
    m_gpioPin = gpioPin;
}

void PixelBuffer::initialize()
{
    // Back-compat: deduce the number of rows/columns from the m_rows and m_columns vectors.
    // Initialize all entries to -1 to indicate no pixel at that location.
    //m_pixelMap.resize(m_rows.size(), std::vector<int>(m_columns.size(), -1));
    for (uint i = 0; i < m_rows.size(); i++)
    {
        m_pixelMap.push_back(std::vector<int>(m_columns.size(), -1));
        m_colorMap.push_back(std::vector<ulong>(m_columns.size(), 0));
    }

    // Initialize the color map to black (0) for all pixels.
    //m_colorMap.resize(m_rows.size(), std::vector<ulong>(m_columns.size(), 0));
    
    // More back-compat: populate the pixel map based on the row/column vectors.
    for (int pixelNumber = 0; pixelNumber < m_numPixels; pixelNumber++)
    {
        for (int row = 0; row < m_rows.size(); row++)
        {
            std::vector<int>* rowPixels = m_rows.at(row);
            if (std::find(rowPixels->begin(), rowPixels->end(), pixelNumber) != rowPixels->end())
            {
                // Found the pixel in this row.
                for (int column = 0; column < m_columns.size(); column++)
                {
                    std::vector<int>* columnPixels = m_columns.at(column);
                    if (std::find(columnPixels->begin(), columnPixels->end(), pixelNumber) != columnPixels->end())
                    {
                        // Found the pixel in this column.
                        setRowAndColumnForPixel(pixelNumber, row, column);
                        break;
                    }
                }

                break;
            }
        }
    }

    m_neoPixels = new Adafruit_NeoPixel(m_pixelBufferSize, m_gpioPin, NEO_GRB + NEO_KHZ800);
    clearBuffer();
    m_neoPixels->begin();
    m_neoPixels->clear();
}

void PixelBuffer::setRowAndColumnForPixel(uint pixel, uint row, uint column)
{
    if (row >= m_pixelMap.size())
    {
        return;
    }

    if (column >= m_pixelMap[row].size())
    {
        return;
    }

    m_pixelMap[row][column] = pixel;
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

    while (!m_neoPixels->canShow())
    {
        // wait for the "show" to complete
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

uint PixelBuffer::getDigitCount()
{
    return m_digits.size();
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

void PixelBuffer::setRowColor(uint row, ulong newColor)
{
    if (row >= m_rows.size())
    {
        return;
    }

    std::vector<int> *destination = m_rows.at(row);
    setColorForMappedPixels(destination, newColor);
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
    // shiftPixelBlocksRight(m_columns, newColor, 0);

    // Shift all columns to the right, then fill the first column with the new color.
    for (uint row = 0; row < m_colorMap.size(); row++)
    {
        for (uint col = m_columns.size() - 1; col > 0; col--)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col-1]);
        }

        setColorInPixelMap(row, 0, newColor);
    }
}

void PixelBuffer::shiftColumnsRight(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftColumnsLeft(ulong newColor)
{
    // Shift all columns to the left, then fill the last column with the new color.
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
    return;

    // Shift all rows down, then fill the first row with the new color.
    // Todo: Make this cleaner by setting a RowCount/ColumnCount property.
    // For now, rely on the count of m_rows / m_columns.
    for (uint col = 0; col < m_columns.size(); col++)
    {
        for (uint row = m_rows.size() - 1; row > 0; row--)
        {
            setColorInPixelMap(row, col, m_colorMap[row-1][col]);
        }

        setColorInPixelMap(0, col, newColor);
    }
}

void PixelBuffer::shiftRowsDown(ulong newColor, uint startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftDigitsRight(ulong newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
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

void PixelBuffer::setColorInPixelMap(uint row, uint column, ulong color)
{
    // Since this is private for now, we could remove the guard clauses for better performance.
    if (row >= m_colorMap.size())
    {
        return;
    }

    if (column >= m_colorMap[row].size())
    {
        return;
    }

    m_colorMap[row][column] = color;
    if (m_pixelMap[row][column] != -1)
    {
        m_pixelColors[m_pixelMap[row][column]] = color;
    }
}

const std::vector<std::vector<int>*>& PixelBuffer::getAllRows()
{
    return m_rows;
}

const std::vector<std::vector<int>*>& PixelBuffer::getAllColumns()
{
    return m_columns;
}

const std::vector<std::vector<int>*> PixelBuffer::getRowsForDigit(uint digit)
{
    std::vector<std::vector<int>*> rowsForDigit;
    if (digit >= m_digits.size())
    {
        return rowsForDigit;
    }

    // Loop over all rows, find out which rows exist in the digit and have pixels in the digit.
    std::vector<int>* pixelsInDigit = m_digits[digit];
    for (std::vector<int>* row : m_rows)
    {
        std::vector<int>* rowInDigit = new std::vector<int>();
        rowsForDigit.push_back(rowInDigit);

        for (int pixel : *row)
        {
            if (std::find(pixelsInDigit->begin(), pixelsInDigit->end(), pixel) != pixelsInDigit->end())
            {
                rowInDigit->push_back(pixel);
            }
        }
    }

    return rowsForDigit;
}