#include "RadialDisplayPattern.h"

RadialDisplayPattern::RadialDisplayPattern() : DisplayPattern()
{
}

void RadialDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    uint16_t columnCount = pixelMap->getColumnCount();
    uint16_t colsToLeft = pixelMap->getColsToLeft();
    uint16_t colsToRight = pixelMap->getColsToRight();
    uint16_t rowCount = pixelMap->getRowCount();
    uint16_t colCount = pixelMap->getColumnCount();

    // Simple logic for now:
    // Center point is only this display, not including offests.
    // Initialize the radial color list with the first colors from the color pattern.
    m_colorPattern->reset();
    m_centerColumn = (colsToLeft + columnCount + colsToRight) / 2.0f;
    m_centerRow = (rowCount - 1) / 2.0f;

    m_radialColors.clear();
    m_pixelDistances.clear();

    // Initialize the radial colors based on the maximum distance from center.
    float xMax = 0.0f;
    if (m_centerColumn <= colsToLeft)
    {
        xMax = (colsToLeft - m_centerColumn + columnCount);
    } 
    else if(m_centerColumn >= (colsToLeft + columnCount))
    {
        xMax = (m_centerColumn - colsToLeft);
    } 
    else
    {
        xMax = std::max(m_centerColumn - colsToLeft, (colsToLeft + columnCount) - m_centerColumn);
    }

    float yMax = m_centerRow;

    uint32_t startingColor = m_colorPattern->getNextColor();
    int maxRadius = static_cast<int>(std::sqrt(xMax * xMax + yMax * yMax)) + 1;
    for (int i = 0; i <= maxRadius; i++)
    {
        m_radialColors.push_back(startingColor);
    }

    // Initialize the distance map for each pixel.
    for (int row = 0; row < rowCount; row++)
    {
        std::vector<float> distancesInRow;
        for (int col = 0; col < colCount; col++)
        {
            float deltaY = row - m_centerRow;
            float deltaX = col + colsToLeft - m_centerColumn;
            float distanceToPixel = std::sqrt(deltaX * deltaX + deltaY * deltaY);
            distancesInRow.push_back(distanceToPixel);
        }

        m_pixelDistances.push_back(distancesInRow);
    }
}

void RadialDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    // Update the radial color map:
    // Shift all colors outward by 1 distance unit, getting a new color at distance 0.
    for (int i = m_radialColors.size() - 1; i >= 1; i--)
    {
        m_radialColors[i] = m_radialColors[i - 1];
    }
    m_radialColors[0] = m_colorPattern->getNextColor();

    // Now update the pixel map based on the distance map and radial colors.
    uint16_t rowCount = pixelMap->getRowCount();
    uint16_t colCount = pixelMap->getColumnCount();
    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < colCount; col++)
        {
            float distanceToPixel = m_pixelDistances[row][col];
            int distanceIndex = static_cast<int>(distanceToPixel + 0.5f);
            if (distanceIndex >= m_radialColors.size())
            {
                // Shouldn't happen, but just in case...
                distanceIndex = m_radialColors.size() - 1;
            }

            uint32_t colorForPixel = m_radialColors[distanceIndex];
            pixelMap->setColorInPixelMap(row, col, colorForPixel);
        }
    }
}

