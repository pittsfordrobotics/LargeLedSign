#include "PixelBuffer.h"

PixelBuffer::PixelBuffer(const DisplayConfiguration* displayConfiguration)
{
    // Todo: Could stash the config copy and reference the row-pixel, col-pixel, etc, maps
    // directly from the config object instead of copying them to internal members.
    DisplayConfiguration config(*displayConfiguration);

    m_numPixels = config.getNumberOfPixels();
    m_pixelBuffer = new unsigned long[m_numPixels];
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

void PixelBuffer::clearBuffer()
{
    for (unsigned int i = 0; i < m_numPixels; i++)
    {
        m_pixelBuffer[i] = 0;
    }
}


unsigned int PixelBuffer::getColumnCount()
{
    return m_columns.size();
}

unsigned int PixelBuffer::getRowCount()
{
    return m_rows.size();
}

unsigned int PixelBuffer::getPixelCount()
{
    return m_numPixels;
}

void PixelBuffer::setPixel(unsigned int pixel, unsigned long color)
{
    if (pixel >= m_numPixels)
    {
        return;
    }

    m_pixelBuffer[pixel] = color;
}

unsigned long PixelBuffer::getPixel(unsigned int pixel)
{
    if (pixel >= m_numPixels)
    {
        return 0;
    }

    return m_pixelBuffer[pixel];
}

void PixelBuffer::fill(unsigned long newColor)
{
    for (unsigned int i = 0; i < m_numPixels; i++)
    {
        m_pixelBuffer[i] = newColor;
    }
}

void PixelBuffer::fillRandomly(unsigned long newColor, unsigned int numberOfPixels)
{
    for (unsigned int i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelBuffer[pixel] = newColor;
    }
}

void PixelBuffer::shiftPixelsRight(unsigned long newColor)
{
    for (unsigned int i = m_numPixels - 1; i >= 1; i--)
    {
        m_pixelBuffer[i] = m_pixelBuffer[i - 1];
    }

    m_pixelBuffer[0] = newColor;
}

void PixelBuffer::shiftPixelsLeft(unsigned long newColor)
{
    for (unsigned int i = 0; i < m_numPixels - 1; i++)
    {
        m_pixelBuffer[i] = m_pixelBuffer[i + 1];
    }

    m_pixelBuffer[m_numPixels - 1] = newColor;
}

void PixelBuffer::shiftColumnsRight(unsigned long newColor)
{
    shiftPixelBlocksRight(m_columns, newColor, 0);
}

void PixelBuffer::shiftColumnsRight(unsigned long newColor, unsigned int startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftColumnsLeft(unsigned long newColor)
{
    shiftPixelBlocksLeft(m_columns, newColor, m_columns.size() - 1);
}

void PixelBuffer::shiftColumnsLeft(unsigned long newColor, unsigned int startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftRowsUp(unsigned long newColor)
{
    shiftPixelBlocksLeft(m_rows, newColor, m_rows.size() - 1);
}

void PixelBuffer::shiftRowsUp(unsigned long newColor, unsigned int startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftRowsDown(unsigned long newColor)
{
    shiftPixelBlocksRight(m_rows, newColor, 0);
}

void PixelBuffer::shiftRowsDown(unsigned long newColor, unsigned int startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftDigitsRight(unsigned long newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
}

void PixelBuffer::shiftLine(unsigned long newColor)
{
    for (unsigned int i = m_numPixels - 1; i > 0; i--)
    {
        m_pixelBuffer[i] = m_pixelBuffer[i - 1];
    }

    m_pixelBuffer[0] = newColor;
}

void PixelBuffer::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, unsigned long newColor, unsigned int startingBlock)
{
    for (unsigned int i = pixelBlocks.size() - 1; i > startingBlock; i--)
    {
        std::vector<int> *source = pixelBlocks.at(i - 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelBuffer[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, unsigned long newColor, unsigned int startingBlock)
{
    for (unsigned int i = 0; i < startingBlock; i++)
    {
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelBuffer[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer::setColorForMappedPixels(std::vector<int> *destination, unsigned long newColor)
{
    for (unsigned int i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        m_pixelBuffer[pixelIndex] = newColor;
    }
}