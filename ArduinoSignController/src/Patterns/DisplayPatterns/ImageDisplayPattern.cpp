#include "ImageDisplayPattern.h"

ImageDisplayPattern::ImageDisplayPattern()
{
}

void ImageDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    pixelMap->clearBuffer();
    FileReader fileReader;

    std::vector<uint8_t> bytes = fileReader.getFileBytes("test.bmp");
    std::vector<std::vector<unsigned long>> pixelColors = BmpUtils::ParseBmp(bytes);
    
    if (pixelColors.empty())
    {
        DebugUtils::println("Failed to read or parse bitmap for ImageDisplayPattern.");
        return;
    }

    int maxRow = std::min((int)pixelColors.size(), (int)pixelMap->getRowCount());
    int maxCol = std::min((int)pixelColors[0].size(), (int)pixelMap->getColumnCount());

    for (int row = 0; row < maxRow; row++)
    {
        for (int col = 0; col < maxCol; col++)
        {
            pixelMap->setColorInPixelMap(row, col, pixelColors[row][col]);
        }
    }
}

void ImageDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    // No dynamic behavior to update for this pattern.
}

std::vector<String> ImageDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}