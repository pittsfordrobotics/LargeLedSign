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

    switch (shiftType)
    {
        case ImageDisplayShiftType::Right:
            edgeRowOrCol = 0;
            break;
        case ImageDisplayShiftType::Left:
            edgeRowOrCol = maxCol - 1;
            break;
        case ImageDisplayShiftType::Up:
            edgeRowOrCol = maxRow - 1;
            break;
        case ImageDisplayShiftType::Down:
            edgeRowOrCol = 0;
            break;
        default:
            // Not using shift, don't bother recording current row/col.
            break;
    }
}

void ImageDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    if (imageData.empty())
    {
        return;
    }

    int numImageRows = imageData.size();
    int numImageCols = imageData[0].size();
    int maxRow = std::min(numImageRows, (int)pixelMap->getRowCount());
    int maxCol = std::min(numImageCols, (int)pixelMap->getColumnCount());

    switch (shiftType)
    {
        case ImageDisplayShiftType::Right:
            pixelMap->shiftColumnsRight();
            edgeRowOrCol = (edgeRowOrCol - 1 + numImageCols) % numImageCols;
            for (int row = 0; row < maxRow; row++)
            {
                pixelMap->setColorInPixelMap(row, 0, imageData[row][edgeRowOrCol]);
            }
            break;
        case ImageDisplayShiftType::Left:
            pixelMap->shiftColumnsLeft();
            edgeRowOrCol = (edgeRowOrCol + 1) % numImageCols;
            for (int row = 0; row < maxRow; row++)
            {
                pixelMap->setColorInPixelMap(row, maxCol - 1, imageData[row][edgeRowOrCol]);
            }
            break;
        case ImageDisplayShiftType::Up:
            pixelMap->shiftRowsUp();
            edgeRowOrCol = (edgeRowOrCol + 1) % numImageRows;
            for (int col = 0; col < maxCol; col++)
            {
                pixelMap->setColorInPixelMap(maxRow - 1, col, imageData[edgeRowOrCol][col]);
            }
            break;
        case ImageDisplayShiftType::Down:
            pixelMap->shiftRowsDown();
            edgeRowOrCol = (edgeRowOrCol - 1 + numImageRows) % numImageRows;
            for (int col = 0; col < maxCol; col++)
            {
                pixelMap->setColorInPixelMap(0, col, imageData[edgeRowOrCol][col]);
            }
            break;
        default:
            // Not using shift, so nothing to update.
            break;
    }
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

