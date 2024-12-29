#include "DisplayConfiguration.h"

std::vector<DisplayConfiguration*>* DisplayConfiguration::ParseJson(String jsonString)
{
    std::vector<DisplayConfiguration*>* configs = new std::vector<DisplayConfiguration*>();
    DisplayConfiguration* config = new DisplayConfiguration();
    
    // Todo: actually read the input
    config->m_gpioPin = 16;
    config->m_numPixels = 256;
    config->m_numCols = 8;
    config->m_numRows = 8;
    config->defaultBrightness = 15;

    // // Map the pixel indices to rows and columns.
    // // ROW 0 is at the TOP of the display.
    // // COLUMN 0 is at the LEFT of the display.
    // for (uint16_t col = 7; col >= 0; col--)
    // {
    //     std::vector<uint16_t> *rowVector = new std::vector<uint16_t>();
    //     for (uint16_t row = 0; row < 8; row++)
    //     {
    //         if (row % 2 == 0)
    //         {
    //             rowVector->push_back(row * 8 + col);
    //         }
    //         else
    //         {
    //             rowVector->push_back(row * 8 + (7 - col));
    //         }
    //     }
    //     config->m_rowPixelMapping.push_back(rowVector);
    // }

    // for (uint16_t col = 7; col >= 0; col--)
    // {
    //     std::vector<uint16_t> *colVector = new std::vector<uint16_t>();
    //     for (uint16_t row = 0; row < 8; row++)
    //     {
    //         colVector->push_back(col * 8 + row);
    //     }
    //     config->m_columnPixelMapping.push_back(colVector);
    // }

    // config->m_digitPixelMapping.push_back(new std::vector<uint16_t>());
    
    // for (uint i = 0; i < config->m_numPixels; i++)
    // {
    //     config->m_digitPixelMapping[0]->push_back(i);
    // }

    // configs->push_back(config);

    return configs;
}

DisplayConfiguration::DisplayConfiguration()
{
}

DisplayConfiguration::DisplayConfiguration(const DisplayConfiguration& other)
{
    copy(other);
}

DisplayConfiguration& DisplayConfiguration::operator=(const DisplayConfiguration& other)
{
    if (this != &other)
    {
        copy(other);
    }
    
    return *this;
}

void DisplayConfiguration::copy(const DisplayConfiguration& other)
{
    this->m_gpioPin = other.m_gpioPin;
    this->m_numPixels = other.m_numPixels;
    this->m_numCols = other.m_numCols;
    this->m_numRows = other.m_numRows;
    this->m_colsToLeft = other.m_colsToLeft;
    this->m_colsToRight = other.m_colsToRight;
    this->m_rowsAbove = other.m_rowsAbove;
    this->m_rowsBelow = other.m_rowsBelow;
    this->m_columnPixelMapping = other.m_columnPixelMapping;
    this->m_rowPixelMapping = other.m_rowPixelMapping;
}
