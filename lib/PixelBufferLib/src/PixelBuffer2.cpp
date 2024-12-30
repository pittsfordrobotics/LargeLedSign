#include "PixelBuffer2.h"

PixelBuffer2::PixelBuffer2(const DisplayConfiguration* displayConfiguration)
{
    // Todo: Could stash the config copy and reference the row-pixel, col-pixel, etc, maps
    // directly from the config object instead of copying them to internal members.
    DisplayConfiguration config(*displayConfiguration);

    m_numPixels = config.getNumberOfPixels();
    m_pixelBuffer = new uint32_t[m_numPixels];
    m_digitsToLeft = config.getDigitsToLeft();
    m_digitsToRight = config.getDigitsToRight();
    m_colsToLeft = config.getColumnsToLeft();
    m_colsToRight = config.getColumnsToRight();

    for (int row = 0; row < config.getRowPixelMapping().size(); row++)
    {
        std::vector<int>* rowMap = new std::vector<int>();
        for (int pixel = 0; pixel < config.getRowPixelMapping().at(row)->size(); pixel++)
        {
            rowMap->push_back(config.getRowPixelMapping().at(row)->at(pixel));
        }

        m_rows.push_back(rowMap);
    }

    for (int col = 0; col < config.getColumnPixelMapping().size(); col++)
    {
        std::vector<int>* colMap = new std::vector<int>();
        for (int pixel = 0; pixel < config.getColumnPixelMapping().at(col)->size(); pixel++)
        {
            colMap->push_back(config.getColumnPixelMapping().at(col)->at(pixel));
        }

        m_columns.push_back(colMap);
    }

    for (int digit = 0; digit < config.getDigitPixelMapping().size(); digit++)
    {
        std::vector<int>* digitMap = new std::vector<int>();
        for (int pixel = 0; pixel < config.getDigitPixelMapping().at(digit)->size(); pixel++)
        {
            digitMap->push_back(config.getDigitPixelMapping().at(digit)->at(pixel));
        }

        m_digits.push_back(digitMap);
    }

    clearBuffer();
}

void PixelBuffer2::clearBuffer()
{
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_pixelBuffer[i] = 0;
    }
}


uint PixelBuffer2::getColumnCount()
{
    return m_columns.size();
}

uint PixelBuffer2::getRowCount()
{
    return m_rows.size();
}

uint PixelBuffer2::getPixelCount()
{
    return m_numPixels;
}

void PixelBuffer2::setPixel(unsigned int pixel, ulong color)
{
    if (pixel >= m_numPixels)
    {
        return;
    }

    m_pixelBuffer[pixel] = color;
}

ulong PixelBuffer2::getPixel(uint pixel)
{
    if (pixel >= m_numPixels)
    {
        return 0;
    }

    return m_pixelBuffer[pixel];
}

void PixelBuffer2::fill(ulong newColor)
{
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_pixelBuffer[i] = newColor;
    }
}

void PixelBuffer2::fillRandomly(ulong newColor, uint numberOfPixels)
{
    for (uint i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelBuffer[pixel] = newColor;
    }
}

void PixelBuffer2::shiftPixelsRight(ulong newColor)
{
    for (uint i = m_numPixels - 1; i >= 1; i--)
    {
        m_pixelBuffer[i] = m_pixelBuffer[i - 1];
    }

    m_pixelBuffer[0] = newColor;
}

void PixelBuffer2::shiftPixelsLeft(ulong newColor)
{
    for (uint i = 0; i < m_numPixels - 1; i++)
    {
        m_pixelBuffer[i] = m_pixelBuffer[i + 1];
    }

    m_pixelBuffer[m_numPixels - 1] = newColor;
}

void PixelBuffer2::shiftColumnsRight(ulong newColor)
{
    shiftPixelBlocksRight(m_columns, newColor, 0);
}

void PixelBuffer2::shiftColumnsRight(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelBuffer2::shiftColumnsLeft(ulong newColor)
{
    shiftPixelBlocksLeft(m_columns, newColor, m_columns.size() - 1);
}

void PixelBuffer2::shiftColumnsLeft(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelBuffer2::shiftRowsUp(ulong newColor)
{
    shiftPixelBlocksLeft(m_rows, newColor, m_rows.size() - 1);
}

void PixelBuffer2::shiftRowsUp(ulong newColor, uint startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelBuffer2::shiftRowsDown(ulong newColor)
{
    shiftPixelBlocksRight(m_rows, newColor, 0);
}

void PixelBuffer2::shiftRowsDown(ulong newColor, uint startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

void PixelBuffer2::shiftDigitsRight(ulong newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
}

void PixelBuffer2::shiftLine(ulong newColor)
{
    for (uint i = m_numPixels - 1; i > 0; i--)
    {
        m_pixelBuffer[i] = m_pixelBuffer[i - 1];
    }

    m_pixelBuffer[0] = newColor;
}

void PixelBuffer2::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, ulong newColor, uint startingBlock)
{
    for (uint i = pixelBlocks.size() - 1; i > startingBlock; i--)
    {
        std::vector<int> *source = pixelBlocks.at(i - 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelBuffer[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer2::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, ulong newColor, uint startingBlock)
{
    for (uint i = 0; i < startingBlock; i++)
    {
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelBuffer[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer2::setColorForMappedPixels(std::vector<int> *destination, uint32_t newColor)
{
    for (uint i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        m_pixelBuffer[pixelIndex] = newColor;
    }
}