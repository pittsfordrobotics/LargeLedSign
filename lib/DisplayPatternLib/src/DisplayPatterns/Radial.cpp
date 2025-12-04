#include "Radial.h"

RadialDisplayPattern::RadialDisplayPattern() : DisplayPattern()
{
}

void RadialDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    // Simple logic for now:
    // Center point is only this display, not including offests.
    // Initialize the radial color list with the first colors from the color pattern.
    m_colorPattern->reset();
    m_centerColumn = (pixelMap->getColumnCount()-1) / 2.0f;
    m_centerRow = (pixelMap->getRowCount()-1) / 2.0f;

    m_radialColors.clear();
    m_pixelDistances.clear();

    // Initialize the radial colors based on the maximum distance from center.
    float xMax = m_centerColumn;  // Take offsets into account later
    float yMax = m_centerRow;

    uint32_t startingColor = m_colorPattern->getNextColor();
    int maxRadius = static_cast<int>(std::sqrt(xMax * xMax + yMax * yMax)) + 1;
    for (int i = 0; i <= maxRadius; i++)
    {
        m_radialColors.push_back(startingColor);
    }

    // Initialize the distance map for each pixel.
    for (int row = 0; row < pixelMap->getRowCount(); row++)
    {
        std::vector<float> distancesInRow;
        for (int col = 0; col < pixelMap->getColumnCount(); col++)
        {
            float deltaY = row - m_centerRow;
            float deltaX = col - m_centerColumn;
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
    for (int row = 0; row < pixelMap->getRowCount(); row++)
    {
        for (int col = 0; col < pixelMap->getColumnCount(); col++)
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

