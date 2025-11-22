#include "PixelMap.h"

PixelMap::PixelMap(const DisplayConfiguration& displayConfiguration)
{
    initializeFromConfiguration(displayConfiguration);
    initializeInternalMaps();
    clearBuffer();
}

void PixelMap::initializeFromConfiguration(const DisplayConfiguration& displayConfiguration)
{
    DisplayConfiguration config(displayConfiguration);

    m_numPixels = config.getNumberOfPixels();
    m_pixelColors = new unsigned long[m_numPixels];
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
}

void PixelMap::initializeInternalMaps()
{
    for (uint i = 0; i < m_rows.size(); i++)
    {
        // Initialize pixel map to -1 (no pixel)
        m_pixelMap.push_back(std::vector<int>(m_columns.size(), -1));
        // Initialize color map to 0 (black)
        m_colorMap.push_back(std::vector<ulong>(m_columns.size(), 0));
    }

    // Populate the pixel map based on the row/column vectors.
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
}

void PixelMap::setRowAndColumnForPixel(uint pixel, uint row, uint column)
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

void PixelMap::clearBuffer()
{
    for (unsigned int i = 0; i < m_numPixels; i++)
    {
        m_pixelColors[i] = 0;
    }
}

unsigned long PixelMap::getPixel(unsigned int pixel)
{
    if (pixel >= m_numPixels)
    {
        return 0;
    }

    return m_pixelColors[pixel];
}

unsigned int PixelMap::getColumnCount()
{
    return m_columns.size();
}

unsigned int PixelMap::getRowCount()
{
    return m_rows.size();
}

unsigned int PixelMap::getDigitCount()
{
    return m_digits.size();
}

unsigned int PixelMap::getPixelCount()
{
    return m_numPixels;
}

void PixelMap::setPixel(unsigned int pixel, unsigned long color)
{
    if (pixel >= m_numPixels)
    {
        return;
    }

    m_pixelColors[pixel] = color;
}

void PixelMap::setRowColor(unsigned int row, unsigned long newColor)
{
    if (row >= m_rows.size())
    {
        return;
    }

    for (unsigned int col = 0; col < m_columns.size(); col++)
    {
        setColorInPixelMap(row, col, newColor);
    }
}

void PixelMap::setColumnColor(unsigned int column, unsigned long newColor)
{
    if (column >= m_columns.size())
    {
        return;
    }

    for (unsigned int row = 0; row < m_rows.size(); row++)
    {
        setColorInPixelMap(row, column, newColor);
    }
}

void PixelMap::fill(unsigned long newColor)
{
    for (unsigned int i = 0; i < m_numPixels; i++)
    {
        m_pixelColors[i] = newColor;
    }
}

void PixelMap::fillRandomly(unsigned long newColor, unsigned int numberOfPixels)
{
    for (unsigned int i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelColors[pixel] = newColor;
    }
}

void PixelMap::shiftPixelsRight(unsigned long newColor)
{
    for (unsigned int i = m_numPixels - 1; i >= 1; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelMap::shiftPixelsLeft(unsigned long newColor)
{
    for (unsigned int i = 0; i < m_numPixels - 1; i++)
    {
        m_pixelColors[i] = m_pixelColors[i + 1];
    }

    m_pixelColors[m_numPixels - 1] = newColor;
}

void PixelMap::shiftColumnsRight(unsigned long newColor)
{
    // Shift all columns to the right, then fill the first column with the new color.
    for (unsigned int row = 0; row < m_rows.size(); row++)
    {
        for (unsigned int col = m_columns.size() - 1; col > 0; col--)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col-1]);
        }

        setColorInPixelMap(row, 0, newColor);
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftColumnsRight(unsigned long newColor, unsigned int startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelMap::shiftColumnsLeft(unsigned long newColor)
{
    // Shift all columns to the left, then fill the last column with the new color.
    for (unsigned int row = 0; row < m_rows.size(); row++)
    {
        for (unsigned int col = 0; col < m_columns.size() - 1; col++)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col+1]);
        }

        setColorInPixelMap(row, m_columns.size() - 1, newColor);
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftColumnsLeft(unsigned long newColor, unsigned int startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelMap::shiftRowsUp(unsigned long newColor)
{
    // Shift all rows up, then fill the last row with the new color.
    for (unsigned int col = 0; col < m_columns.size(); col++)
    {
        for (unsigned int row = 0; row < m_rows.size() - 1; row++)
        {
            setColorInPixelMap(row, col, m_colorMap[row+1][col]);
        }

        setColorInPixelMap(m_rows.size() - 1, col, newColor);
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftRowsUp(unsigned long newColor, unsigned int startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelMap::shiftRowsDown(unsigned long newColor)
{
    // Shift all rows down, then fill the first row with the new color.
    for (unsigned int col = 0; col < m_columns.size(); col++)
    {
        for (unsigned int row = m_rows.size() - 1; row > 0; row--)
        {
            setColorInPixelMap(row, col, m_colorMap[row-1][col]);
        }

        setColorInPixelMap(0, col, newColor);
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftRowsDown(unsigned long newColor, unsigned int startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftDigitsRight(unsigned long newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
}

void PixelMap::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, unsigned long newColor, unsigned int startingBlock)
{
    for (unsigned int i = pixelBlocks.size() - 1; i > startingBlock; i--)
    {
        std::vector<int> *source = pixelBlocks.at(i - 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        if (source->size() == 0 || destination->size() == 0)
        {
            // source or destination is empty -- skip it.
            // this is an artifact of testing the new matrix code using empty rows/columns.
            continue;
        }

        // Find the color of the first pixel in the source column, and set the destination column to that color.
        unsigned long previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    if (startingBlock < pixelBlocks.size())
    {
        setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
    }
}

void PixelMap::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, unsigned long newColor, unsigned int startingBlock)
{
    for (unsigned int i = 0; i < startingBlock; i++)
    {
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        unsigned long previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    if (startingBlock < pixelBlocks.size())
    {
        setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
    }
}

void PixelMap::setColorForMappedPixels(std::vector<int> *destination, unsigned long newColor)
{
    for (unsigned int i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        m_pixelColors[pixelIndex] = newColor;
    }
}

void PixelMap::setColorInPixelMap(unsigned int row, unsigned int column, unsigned long color)
{
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

const std::vector<std::vector<int>*>& PixelMap::getAllRows()
{
    return m_rows;
}

const std::vector<std::vector<int>*>& PixelMap::getAllColumns()
{
    return m_columns;
}

const std::vector<std::vector<int>*> PixelMap::getRowsForDigit(unsigned int digit)
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