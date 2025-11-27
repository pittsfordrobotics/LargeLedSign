#include "PixelMap.h"

PixelMap::PixelMap(const DisplayConfiguration& displayConfiguration)
{
    initializeFromConfiguration(displayConfiguration);
    initializeInternalMaps();
    clearBuffer();
}

PixelMap::~PixelMap()
{
    // Free the pixel color buffer
    if (m_pixelColors)
    {
        delete[] m_pixelColors;
        m_pixelColors = nullptr;
    }
    
    // Free all dynamically allocated vectors in m_rows
    for (std::vector<int>* row : m_rows)
    {
        delete row;
    }
    m_rows.clear();
    
    // Free all dynamically allocated vectors in m_columns
    for (std::vector<int>* col : m_columns)
    {
        delete col;
    }
    m_columns.clear();
    
    // Free all dynamically allocated vectors in m_digits
    for (std::vector<int>* digit : m_digits)
    {
        delete digit;
    }
    m_digits.clear();
}

void PixelMap::initializeFromConfiguration(const DisplayConfiguration& displayConfiguration)
{
    DisplayConfiguration config(displayConfiguration);

    m_numPixels = config.getNumberOfPixels();
    m_pixelColors = new uint32_t[m_numPixels];
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
    for (uint16_t i = 0; i < m_rows.size(); i++)
    {
        // Initialize pixel map to -1 (no pixel)
        m_pixelMap.push_back(std::vector<int>(m_columns.size(), -1));
        // Initialize color map to 0 (black)
        m_colorMap.push_back(std::vector<uint32_t>(m_columns.size(), 0));
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

void PixelMap::setRowAndColumnForPixel(uint16_t pixel, uint16_t row, uint16_t column)
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
    for (uint16_t i = 0; i < m_numPixels; i++)
    {
        m_pixelColors[i] = 0;
    }
}

uint32_t PixelMap::getRawPixelColor(uint16_t pixel)
{
    if (pixel >= m_numPixels)
    {
        return 0;
    }

    return m_pixelColors[pixel];
}

uint16_t PixelMap::getColumnCount()
{
    return m_columns.size();
}

uint16_t PixelMap::getRowCount()
{
    return m_rows.size();
}

uint16_t PixelMap::getDigitCount()
{
    return m_digits.size();
}

uint16_t PixelMap::getPixelCount()
{
    return m_numPixels;
}

void PixelMap::setRawPixel(uint16_t pixel, uint32_t color)
{
    if (pixel >= m_numPixels)
    {
        return;
    }

    m_pixelColors[pixel] = color;
}

void PixelMap::setRowColor(uint16_t row, uint32_t newColor)
{
    if (row >= m_rows.size())
    {
        return;
    }

    for (uint16_t col = 0; col < m_columns.size(); col++)
    {
        setColorInPixelMap(row, col, newColor);
    }
}

void PixelMap::setColumnColor(uint16_t column, uint32_t newColor)
{
    if (column >= m_columns.size())
    {
        return;
    }

    for (uint16_t row = 0; row < m_rows.size(); row++)
    {
        setColorInPixelMap(row, column, newColor);
    }
}

void PixelMap::setDigitColor(uint16_t digit, uint32_t newColor)
{
    if (digit >= m_digits.size())
    {
        return;
    }

    std::vector<int>* digitPixels = m_digits.at(digit);
    for (int pixelIndex : *digitPixels)
    {
        setRawPixel(pixelIndex, newColor);
    }
}

void PixelMap::fill(uint32_t newColor)
{
    for (uint16_t i = 0; i < m_numPixels; i++)
    {
        m_pixelColors[i] = newColor;
    }
}

void PixelMap::fillRandomly(uint32_t newColor, uint16_t numberOfPixels)
{
    for (uint16_t i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelColors[pixel] = newColor;
    }
}

void PixelMap::shiftPixelsRight(uint32_t newColor)
{
    // Use signed int to avoid underflow when decrementing from 0
    for (int i = m_numPixels - 1; i >= 1; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelMap::shiftPixelsRight()
{
    // Use signed int to avoid underflow when decrementing from 0
    for (int i = m_numPixels - 1; i >= 1; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }
}

void PixelMap::shiftPixelsLeft(uint32_t newColor)
{
    for (uint16_t i = 0; i < m_numPixels - 1; i++)
    {
        m_pixelColors[i] = m_pixelColors[i + 1];
    }

    m_pixelColors[m_numPixels - 1] = newColor;
}

void PixelMap::shiftPixelsLeft()
{
    for (uint16_t i = 0; i < m_numPixels - 1; i++)
    {
        m_pixelColors[i] = m_pixelColors[i + 1];
    }
}

void PixelMap::shiftColumnsRight()
{
    if (m_columns.size() == 0) return;  // Safety check
    
    for (int row = 0; row < m_rows.size(); row++)
    {
        for (int col = m_columns.size() - 1; col > 0; col--)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col-1]);
        }
    }
}

void PixelMap::shiftColumnsRight(uint32_t newColor)
{
    if (m_columns.size() == 0) return;  // Safety check
    
    // Shift all columns to the right, then fill the first column with the new color.
    for (uint16_t row = 0; row < m_rows.size(); row++)
    {
        // Use signed int to prevent underflow
        for (int col = m_columns.size() - 1; col > 0; col--)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col-1]);
        }

        setColorInPixelMap(row, 0, newColor);
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftColumnsRight(uint32_t newColor, uint16_t startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelMap::shiftColumnsLeft()
{
    for (int row = 0; row < m_rows.size(); row++)
    {
        for (int col = 0; col < m_columns.size() - 1; col++)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col+1]);
        }
    }
}

void PixelMap::shiftColumnsLeft(uint32_t newColor)
{
    // Shift all columns to the left, then fill the last column with the new color.
    for (uint16_t row = 0; row < m_rows.size(); row++)
    {
        for (uint16_t col = 0; col < m_columns.size() - 1; col++)
        {
            setColorInPixelMap(row, col, m_colorMap[row][col+1]);
        }

        setColorInPixelMap(row, m_columns.size() - 1, newColor);
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftColumnsLeft(uint32_t newColor, uint16_t startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelMap::shiftRowsUp(uint32_t newColor)
{
    // Shift all rows up, then fill the last row with the new color.
    for (uint16_t col = 0; col < m_columns.size(); col++)
    {
        for (uint16_t row = 0; row < m_rows.size() - 1; row++)
        {
            setColorInPixelMap(row, col, m_colorMap[row+1][col]);
        }

        setColorInPixelMap(m_rows.size() - 1, col, newColor);
    }
}

void PixelMap::shiftRowsUp()
{
    for (int col = 0; col < m_columns.size(); col++)
    {
        for (int row = 0; row < m_rows.size() - 1; row++)
        {
            setColorInPixelMap(row, col, m_colorMap[row+1][col]);
        }
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftRowsUp(uint32_t newColor, uint16_t startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelMap::shiftRowsDown(uint32_t newColor)
{
    if (m_rows.size() == 0) return;  // Safety check
    
    // Shift all rows down, then fill the first row with the new color.
    for (uint16_t col = 0; col < m_columns.size(); col++)
    {
        // Use signed int to prevent underflow
        for (int row = m_rows.size() - 1; row > 0; row--)
        {
            setColorInPixelMap(row, col, m_colorMap[row-1][col]);
        }

        setColorInPixelMap(0, col, newColor);
    }
}

void PixelMap::shiftRowsDown()
{
    if (m_rows.size() == 0) return;  // Safety check
    
    for (int col = 0; col < m_columns.size(); col++)
    {
        for (int row = m_rows.size() - 1; row > 0; row--)
        {
            setColorInPixelMap(row, col, m_colorMap[row-1][col]);
        }
    }
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftRowsDown(uint32_t newColor, uint16_t startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

// Update to use the new pixel map.
// Currently only used by the CenterOut pattern.
void PixelMap::shiftDigitsRight(uint32_t newColor)
{
    shiftPixelBlocksRight(m_digits, newColor, 0);
}

void PixelMap::shiftDigitsRight()
{
    if (m_digits.size() < 2) 
    {
        // If less than 2 digits, nothing to shift.
        return;
    }

    // Assumption: all pixels in a digit are the same color.
    for (int digit = m_digits.size() - 1; digit > 0; digit--)
    {
        if (m_digits.at(digit - 1)->size() == 0)
        {
            // Empty digit we can't shift colors if there's nothing there.
            // Shouldn't happen in practice -- just exit.
            return;
        }

        uint32_t previousColor = m_pixelColors[m_digits.at(digit - 1)->at(0)];
        setDigitColor(digit, previousColor);
    }
}

void PixelMap::shiftDigitsLeft()
{
    if (m_digits.size() < 2) 
    {
        // If less than 2 digits, nothing to shift.
        return;
    }

    // Assumption: all pixels in a digit are the same color.
    for (int digit = 0; digit < m_digits.size() - 1; digit++)
    {
        if (m_digits.at(digit + 1)->size() == 0)
        {
            // Empty digit we can't shift colors if there's nothing there.
            // Shouldn't happen in practice -- just exit.
            return;
        }

        uint32_t previousColor = m_pixelColors[m_digits.at(digit + 1)->at(0)];
        setDigitColor(digit, previousColor);
    }
}

void PixelMap::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, uint32_t newColor, uint16_t startingBlock)
{
    if (pixelBlocks.size() == 0) return;  // Safety check
    
    for (int i = pixelBlocks.size() - 1; i > (int)startingBlock; i--)
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
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    if (startingBlock < pixelBlocks.size())
    {
        setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
    }
}

void PixelMap::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, uint32_t newColor, uint16_t startingBlock)
{
    // Ensure we don't go beyond the array bounds
    uint16_t endBlock = (startingBlock < pixelBlocks.size()) ? startingBlock : pixelBlocks.size() - 1;
    
    for (uint16_t i = 0; i < endBlock; i++)
    {
        if (i + 1 >= pixelBlocks.size()) break;  // Safety check
        
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        
        if (source->size() == 0 || destination->size() == 0) continue;  // Skip empty blocks
        
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    if (startingBlock < pixelBlocks.size())
    {
        setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
    }
}

void PixelMap::setColorForMappedPixels(std::vector<int> *destination, uint32_t newColor)
{
    if (!destination) return;  // Null pointer check
    
    for (uint16_t i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        // Critical: validate pixelIndex is within bounds
        if (pixelIndex >= 0 && pixelIndex < m_numPixels)
        {
            m_pixelColors[pixelIndex] = newColor;
        }
    }
}

void PixelMap::setColorInPixelMap(uint16_t row, uint16_t column, uint32_t color)
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
    int pixelIndex = m_pixelMap[row][column];
    // Critical: validate pixelIndex is within bounds before array access
    if (pixelIndex >= 0 && pixelIndex < m_numPixels)
    {
        m_pixelColors[pixelIndex] = color;
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

const std::vector<std::vector<int>*> PixelMap::getRowsForDigit(uint16_t digit)
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