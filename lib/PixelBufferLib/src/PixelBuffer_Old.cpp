#include "PixelBuffer_old.h"

// All pixel buffers will have at least this many pixels, even if not used.
// This is done to help the signs stay in sync.
unsigned int minimumPixelsInBuffer = 360;

PixelBuffer_Old::PixelBuffer_Old(int gpioPin)
{
    m_gpioPin = gpioPin;
}

PixelBuffer_Old::PixelBuffer_Old(const DisplayConfiguration* displayConfiguration)
{
    // Todo: Could stash the config copy and reference the row-pixel, col-pixel, etc, maps
    // directly from the config object instead of copying them to internal members.
    DisplayConfiguration config(*displayConfiguration);
    
    m_gpioPin = config.getGpioPin();
    m_numPixels = config.getNumberOfPixels();
    m_pixelBufferSize = std::max((unsigned int) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new unsigned long[m_pixelBufferSize];

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

    // Todo: Migrate brightness into initialize method.
    initialize();
}

void PixelBuffer_Old::initialize()
{
}

void PixelBuffer_Old::clearBuffer()
{
    for (unsigned int i = 0; i < m_pixelBufferSize; i++)
    {
        m_pixelColors[i] = 0;
    }
}

void PixelBuffer_Old::stop()
{
    m_isStopped = true;
}

void PixelBuffer_Old::resume()
{
    m_isStopped = false;
}

void PixelBuffer_Old::displayPixels()
{
}

unsigned int PixelBuffer_Old::getColumnCount()
{
    return m_columns.size();
}

unsigned int PixelBuffer_Old::getRowCount()
{
    return m_rows.size();
}

unsigned int PixelBuffer_Old::getPixelCount()
{
    return m_numPixels;
}

void PixelBuffer_Old::setBrightness(byte brightness)
{
}

void PixelBuffer_Old::setPixel(unsigned int pixel, unsigned long color)
{
    if (pixel >= m_pixelBufferSize)
    {
        return;
    }

    m_pixelColors[pixel] = color;
}

void PixelBuffer_Old::fill(unsigned long newColor)
{
    for (unsigned int i = 0; i < m_pixelBufferSize; i++)
    {
        m_pixelColors[i] = newColor;
    }
}

void PixelBuffer_Old::fillRandomly(unsigned long newColor, unsigned int numberOfPixels)
{
    for (unsigned int i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelColors[pixel] = newColor;
    }
}

void PixelBuffer_Old::shiftPixelsRight(unsigned long newColor)
{
    for (unsigned int i = m_pixelBufferSize - 1; i >= 1; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelBuffer_Old::shiftPixelsLeft(unsigned long newColor)
{
    for (unsigned int i = 0; i < m_pixelBufferSize - 1; i++)
    {
        m_pixelColors[i] = m_pixelColors[i + 1];
    }

    m_pixelColors[m_numPixels - 1] = newColor;
}

void PixelBuffer_Old::shiftColumnsRight(unsigned long newColor)
{
    shiftPixelBlocksRight(m_columns, newColor, 0);
}

void PixelBuffer_Old::shiftColumnsRight(unsigned long newColor, unsigned int startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelBuffer_Old::shiftColumnsLeft(unsigned long newColor)
{
    shiftPixelBlocksLeft(m_columns, newColor, m_columns.size() - 1);
}

void PixelBuffer_Old::shiftColumnsLeft(unsigned long newColor, unsigned int startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelBuffer_Old::shiftRowsUp(unsigned long newColor)
{
    shiftPixelBlocksLeft(m_rows, newColor, m_rows.size() - 1);
}

void PixelBuffer_Old::shiftRowsUp(unsigned long newColor, unsigned int startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelBuffer_Old::shiftRowsDown(unsigned long newColor)
{
    shiftPixelBlocksRight(m_rows, newColor, 0);
}

void PixelBuffer_Old::shiftRowsDown(unsigned long newColor, unsigned int startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

void PixelBuffer_Old::shiftDigitsRight(unsigned long newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
}

void PixelBuffer_Old::shiftLine(unsigned long newColor)
{
    for (unsigned int i = m_pixelBufferSize - 1; i > 0; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelBuffer_Old::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, unsigned long newColor, unsigned int startingBlock)
{
    for (unsigned int i = pixelBlocks.size() - 1; i > startingBlock; i--)
    {
        std::vector<int> *source = pixelBlocks.at(i - 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer_Old::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, unsigned long newColor, unsigned int startingBlock)
{
    for (unsigned int i = 0; i < startingBlock; i++)
    {
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer_Old::setColorForMappedPixels(std::vector<int> *destination, unsigned long newColor)
{
    for (unsigned int i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        m_pixelColors[pixelIndex] = newColor;
    }
}