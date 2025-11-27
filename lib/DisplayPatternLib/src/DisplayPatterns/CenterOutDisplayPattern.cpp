#include "CenterOutDisplayPattern.h"

CenterOutDisplayPattern::CenterOutDisplayPattern(CenterOutOrientation orientation) : DisplayPattern()
{
    m_orientation = orientation;
}

std::vector<String> CenterOutDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}

void CenterOutDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_colorPattern->reset();
    m_centerColumn = (pixelMap->getColsToLeft() + pixelMap->getColsToRight() + pixelMap->getColumnCount()) / 2 - 1;

    m_centerRow = pixelMap->getRowCount() / 2 - 1;

    if (m_orientation == CenterOutOrientation::Vertical)
    {
        int rowsToFill = pixelMap->getRowCount() - m_centerRow;
        for (int i = 0; i < rowsToFill; i++)
        {
            updateInternal(pixelMap);
        }
    }
    else if (m_orientation == CenterOutOrientation::Horizontal)
    {
        if (m_centerColumn > pixelMap->getColsToLeft() + pixelMap->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            m_colorPattern->incrementOnly(pixelMap->getColsToLeft());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else if (m_centerColumn < pixelMap->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            m_colorPattern->incrementOnly(pixelMap->getColsToRight());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else
        {
            // Center line is inside this display.
            int shiftAmount = std::min(pixelMap->getColsToLeft(), pixelMap->getColsToRight());
            m_colorPattern->incrementOnly(shiftAmount);
            
            int amountToFillLeft = m_centerColumn + 1 - pixelMap->getColsToLeft();
            int amountToFillRight = pixelMap->getColsToLeft() + pixelMap->getColumnCount() - (m_centerColumn + 1);
            int amountToFill = std::max(amountToFillLeft, amountToFillRight);
            
            for (int i = 0; i < amountToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
    }    
    else
    {
        // Just fill with the first color for now.
        pixelMap->fill(m_colorPattern->getNextColor());
    }
}

void CenterOutDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    uint32_t newColor = m_colorPattern->getNextColor();

    switch (m_orientation)
    {
        case CenterOutOrientation::Horizontal:
            updateHorizontal(pixelMap, newColor);
            break;
        case CenterOutOrientation::Vertical:
            updateVertical(pixelMap, newColor);
            break;
        case CenterOutOrientation::Radial:
            updateRadial(pixelMap, newColor);
            break;
    }
}

void CenterOutDisplayPattern::updateVertical(PixelMap* pixelMap, uint32_t newColor)
{
        pixelMap->shiftRowsUp(m_centerColumn);
        pixelMap->setRowColor(m_centerColumn, newColor);
        pixelMap->shiftRowsDown(m_centerColumn + 1);
        pixelMap->setRowColor(m_centerColumn + 1, newColor);
}

void CenterOutDisplayPattern::updateHorizontal(PixelMap* pixelMap, uint32_t newColor)
{
    if (m_centerRow > pixelMap->getColsToLeft() + pixelMap->getColumnCount())
    {
        // Center line is to the right of this display, so we'll only be shifting left.
        pixelMap->shiftColumnsLeft();
        pixelMap->setColumnColor(pixelMap->getColumnCount() - 1, newColor);
    }
    else if (m_centerRow < pixelMap->getColsToLeft())
    {
        // Center line is to the left of this display, so we'll only be shifting right.
        pixelMap->shiftColumnsRight();
        pixelMap->setColumnColor(0, newColor);
    }
    else
    {
        uint16_t localCenter = m_centerRow - pixelMap->getColsToLeft();
        pixelMap->shiftColumnsLeft(localCenter);
        pixelMap->setColumnColor(localCenter, newColor);
        if (localCenter < pixelMap->getColumnCount() - 1) {
            pixelMap->shiftColumnsRight(localCenter + 1);
            pixelMap->setColumnColor(localCenter + 1, newColor);
        }
    }
}

void CenterOutDisplayPattern::updateRadial(PixelMap* pixelMap, uint32_t newColor)
{
    // Simple case for now -- assume the center is within this display.
    pixelMap->shiftColumnsLeft(m_centerColumn);
    pixelMap->shiftColumnsRight(m_centerColumn);
    pixelMap->shiftRowsUp(m_centerRow);
    pixelMap->shiftRowsDown(m_centerRow);
    pixelMap->setColorInPixelMap(m_centerRow, m_centerColumn, newColor);
}
