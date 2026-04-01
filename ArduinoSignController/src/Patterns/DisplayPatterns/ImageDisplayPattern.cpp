#include "ImageDisplayPattern.h"

ImageDisplayPattern::ImageDisplayPattern()
{
    imageData.clear();
}

void ImageDisplayPattern::loadImageData(String filename)
{
    FileReader fileReader;
    std::vector<uint8_t> bytes = fileReader.getFileBytes(filename);
    if (bytes.empty())
    {
        DebugUtils::println("Failed to read bitmap for ImageDisplayPattern. Filename: " + filename);
        imageData.clear();
        return;
    }

    imageData = BmpUtils::ParseBmp(bytes);
}

void ImageDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    pixelMap->clearBuffer();

    String fileName = String(imageNumber) + ".bmp";
    loadImageData(fileName);
    
    if (imageData.empty())
    {
        return;
    }

    int maxRow = std::min((int)imageData.size(), (int)pixelMap->getRowCount());
    int maxCol = std::min((int)imageData[0].size(), (int)pixelMap->getColumnCount());

    for (int row = 0; row < maxRow; row++)
    {
        for (int col = 0; col < maxCol; col++)
        {
            pixelMap->setColorInPixelMap(row, col, imageData[row][col]);
        }
    }
}

void ImageDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    if (imageData.empty())
    {
        return;
    }

    // Switch based on shift type, default to "none".
    // "None" is a static display, so nothing to do.
}

std::vector<String> ImageDisplayPattern::getParameterNames()
{
    return std::vector<String>() = {"Shift Type", "Image Number"};
}

void ImageDisplayPattern::setShiftType(int shiftType)
{
    this->shiftType = static_cast<ImageDisplayShiftType>(shiftType);
}

void ImageDisplayPattern::setImageNumber(int imageNumber)
{
    this->imageNumber = imageNumber;
}

