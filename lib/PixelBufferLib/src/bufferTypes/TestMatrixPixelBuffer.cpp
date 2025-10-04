
#include "TestMatrixPixelBuffer.h"

TestMatrixPixelBuffer::TestMatrixPixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    m_numPixels = 64;
    m_pixelBufferSize = std::max((uint) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    // Map the pixel indices to rows, columns.
    // ROW 0 is at the TOP of the display.
    // COLUMN 0 is at the LEFT of the display.
    for (int col = 7; col >= 0; col--)
    {
        std::vector<int> *rowVector = new std::vector<int>();
        for (int row = 0; row < 8; row++)
        {
            if (row % 2 == 0)
            {
                rowVector->push_back(row * 8 + col);
            }
            else
            {
                rowVector->push_back(row * 8 + (7 - col));
            }
        }
        m_rows.push_back(rowVector);
    }

    for (int col = 7; col >= 0; col--)
    {
        std::vector<int> *colVector = new std::vector<int>();
        for (int row = 0; row < 8; row++)
        {
            colVector->push_back(col * 8 + row);
        }
        m_columns.push_back(colVector);
    }

    m_digits.push_back(new std::vector<int>());
    
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_digits[0]->push_back(i);
    }
}