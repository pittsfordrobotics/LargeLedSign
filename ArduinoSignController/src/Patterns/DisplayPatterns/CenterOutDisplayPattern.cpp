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
    int fullDisplayCenterColumn = (pixelMap->getColsToLeft() + pixelMap->getColsToRight() + pixelMap->getColumnCount()) / 2  - 1;
    m_centerColumn = fullDisplayCenterColumn - pixelMap->getColsToLeft();
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
        if (m_centerColumn > pixelMap->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            m_colorPattern->incrementOnly(pixelMap->getColsToLeft());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else if (m_centerColumn < 0)
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
            
            int amountToFillLeft = m_centerColumn + 1;
            int amountToFillRight = pixelMap->getColumnCount() - (m_centerColumn + 1);
            int amountToFill = std::max(amountToFillLeft, amountToFillRight);
            
            for (int i = 0; i < amountToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
    }    
    else
    {
        // Reset for Square pattern.
        // Ideally we should be taking the number of rows into account, but only using Columns for now.
        if (m_centerColumn < 0)
        {
            // Center line is to the left of this display
            m_colorPattern->incrementOnly(pixelMap->getColsToRight());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else if (m_centerColumn >= pixelMap->getColumnCount())
        {
            // Center line is to the right of this display
            m_colorPattern->incrementOnly(pixelMap->getColsToLeft());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else{
            // Center line is inside this display
            int shiftAmount = std::min(pixelMap->getColsToLeft(), pixelMap->getColsToRight());
            m_colorPattern->incrementOnly(shiftAmount);
            
            int amountToFillLeft = m_centerColumn + 1;
            int amountToFillRight = pixelMap->getColumnCount() - (m_centerColumn + 1);
            int amountToFill = std::max(amountToFillLeft, amountToFillRight);
            
            for (int i = 0; i < amountToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
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
        case CenterOutOrientation::Square:
            updateSquare(pixelMap, newColor);
            break;
    }
}

void CenterOutDisplayPattern::updateVertical(PixelMap* pixelMap, uint32_t newColor)
{
        pixelMap->shiftRowsUp(m_centerRow);
        pixelMap->setRowColor(m_centerRow, newColor);
        pixelMap->shiftRowsDown(m_centerRow + 1);
        pixelMap->setRowColor(m_centerRow + 1, newColor);
}

void CenterOutDisplayPattern::updateHorizontal(PixelMap* pixelMap, uint32_t newColor)
{
    // If the center column is off the display, it will be ignored.
    int columnToShift = MathUtils::clamp(m_centerColumn, 0, pixelMap->getColumnCount()-1);
    pixelMap->shiftColumnsLeft(columnToShift);
    pixelMap->shiftColumnsRight(columnToShift + 1);
    pixelMap->setColumnColor(columnToShift, newColor);
    pixelMap->setColumnColor(columnToShift + 1, newColor);
}

void CenterOutDisplayPattern::updateSquare(PixelMap* pixelMap, uint32_t newColor)
{
    pixelMap->shiftColumnsLeft(m_centerColumn);
    pixelMap->shiftColumnsRight(m_centerColumn + 1);
    pixelMap->shiftRowsUp(m_centerRow);
    pixelMap->shiftRowsDown(m_centerRow + 1);

    // Only looking at the columns here -- we don't have functionality for "rows above/below".
    if (m_centerColumn >= -1 && m_centerColumn < pixelMap->getColumnCount())
    {
        // Center point is inside this display, or is just outside so that center+1 is inside the display.
        pixelMap->setColorInPixelMap(m_centerRow, m_centerColumn, newColor);
        pixelMap->setColorInPixelMap(m_centerRow+1, m_centerColumn, newColor);
        pixelMap->setColorInPixelMap(m_centerRow, m_centerColumn+1, newColor);
        pixelMap->setColorInPixelMap(m_centerRow+1, m_centerColumn+1, newColor);
    }
    else
    {
        // Center column is outside the display.  See if it's to the left or right.
        // Note that when setting pixel colors below, we're relying on the fact that pixels outside of the display are ignored.
        if (m_centerColumn < 0)
        {
            // Center column is to the left of this display, put new colors in the leftmost column.
            int distanceToLeftEdge = -1 * m_centerColumn;
            for (int i = 0; i < distanceToLeftEdge; i++)
            {
                pixelMap->setColorInPixelMap(m_centerRow - i, 0, newColor);
                pixelMap->setColorInPixelMap(m_centerRow + 1 + i, 0, newColor);
            }
        }
        else
        {
            // Center column is to the right of this display, put new colors in the rightmost column.
            int distanceToRightEdge = m_centerColumn - pixelMap->getColumnCount();
            for (int i = 0; i < distanceToRightEdge; i++)
            {
                pixelMap->setColorInPixelMap(m_centerRow - i, pixelMap->getColumnCount() - 1, newColor);
                pixelMap->setColorInPixelMap(m_centerRow + 1 + i, pixelMap->getColumnCount() - 1, newColor);
            }
        }
    }
}
