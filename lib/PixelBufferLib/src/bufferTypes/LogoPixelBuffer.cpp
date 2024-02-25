#include "LogoPixelBuffer.h"

LogoPixelBuffer::LogoPixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    // 140 actual pixels
    // 23 columns, 24 rows
    m_numPixels = 140;
    m_pixelBufferSize = std::max((uint) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    m_columns.push_back(new std::vector<int>{ 99, 100});
    m_columns.push_back(new std::vector<int>{ 98, 102});
    m_columns.push_back(new std::vector<int>{ 95, 96, 97, 101, 103, 104});
    m_columns.push_back(new std::vector<int>{ 91, 93, 94, 105, 106, 107, 109});
    m_columns.push_back(new std::vector<int>{ 89, 90, 92, 108, 110, 111});
    m_columns.push_back(new std::vector<int>{ 7, 88, 112});
    m_columns.push_back(new std::vector<int>{ 13, 25, 87, 113});
    m_columns.push_back(new std::vector<int>{ 8, 19, 26, 86, 114});
    m_columns.push_back(new std::vector<int>{ 2, 9, 14, 27, 31, 33, 85, 115});
    m_columns.push_back(new std::vector<int>{ 0, 20, 29, 34, 35, 84, 116});
    m_columns.push_back(new std::vector<int>{ 3, 15, 28, 32, 82, 83, 117, 118, 119});
    m_columns.push_back(new std::vector<int>{ 1, 10, 21, 30, 52, 78, 80, 81, 120, 121});
    m_columns.push_back(new std::vector<int>{ 4, 16, 36, 47, 57, 77, 79, 122, 123});
    m_columns.push_back(new std::vector<int>{ 11, 22, 42, 53, 61, 68, 124});
    m_columns.push_back(new std::vector<int>{ 5, 17, 37, 48, 58, 65, 76, 125});
    m_columns.push_back(new std::vector<int>{ 12, 23, 43, 54, 62, 69, 73, 126});
    m_columns.push_back(new std::vector<int>{ 6, 18, 38, 49, 59, 66, 74, 127});
    m_columns.push_back(new std::vector<int>{ 24, 44, 55, 63, 70, 128});
    m_columns.push_back(new std::vector<int>{ 39, 50, 60, 71, 75, 129, 130, 132});
    m_columns.push_back(new std::vector<int>{ 45, 56, 64, 67, 131, 133, 134, 135});
    m_columns.push_back(new std::vector<int>{ 40, 51, 72, 136, 137});
    m_columns.push_back(new std::vector<int>{ 46, 138});
    m_columns.push_back(new std::vector<int>{ 41, 139});

    m_rows.push_back(new std::vector<int>{ 119, 120});
    m_rows.push_back(new std::vector<int>{ 118, 122});
    m_rows.push_back(new std::vector<int>{ 117, 121, 124});
    m_rows.push_back(new std::vector<int>{ 111, 115, 116, 123, 125, 129});
    m_rows.push_back(new std::vector<int>{ 113, 114, 126, 127});
    m_rows.push_back(new std::vector<int>{ 109, 110, 112, 128, 130, 131});
    m_rows.push_back(new std::vector<int>{ 108, 132});
    m_rows.push_back(new std::vector<int>{ 0, 1, 107, 133});
    m_rows.push_back(new std::vector<int>{ 2, 3, 4, 5, 6, 106, 134});
    m_rows.push_back(new std::vector<int>{ 7, 8, 9, 10, 11, 12, 105, 135});
    m_rows.push_back(new std::vector<int>{ 13, 14, 15, 16, 17, 18, 104, 136});
    m_rows.push_back(new std::vector<int>{ 19, 20, 21, 22, 23, 24, 25, 27, 102, 103, 137, 139});
    m_rows.push_back(new std::vector<int>{ 26, 28, 36, 37, 38, 39, 40, 41, 98, 100, 101, 138});
    m_rows.push_back(new std::vector<int>{ 29, 30, 42, 43, 44, 45, 46, 97, 99});
    m_rows.push_back(new std::vector<int>{ 31, 32, 47, 48, 49, 50, 51, 96});
    m_rows.push_back(new std::vector<int>{ 33, 34, 52, 53, 54, 55, 56, 95});
    m_rows.push_back(new std::vector<int>{ 35, 57, 58, 59, 60, 94});
    m_rows.push_back(new std::vector<int>{ 61, 62, 63, 64, 93});
    m_rows.push_back(new std::vector<int>{ 65, 66, 67, 92});
    m_rows.push_back(new std::vector<int>{ 68, 69, 70, 71, 72, 88, 90, 91});
    m_rows.push_back(new std::vector<int>{ 73, 74, 75, 86, 87});
    m_rows.push_back(new std::vector<int>{ 76, 77, 81, 83, 84, 85, 89});
    m_rows.push_back(new std::vector<int>{ 78, 82});
    m_rows.push_back(new std::vector<int>{ 79, 80});

    m_digits.push_back(new std::vector<int>());
    
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_digits[0]->push_back(i);
    }    
}