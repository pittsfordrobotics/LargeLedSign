#include "CenterOutDisplayPattern.h"

CenterOutDisplayPattern::CenterOutDisplayPattern(CenterOutOrientation orientation) : DisplayPattern()
{
    m_orientation = orientation;
}

std::vector<String> CenterOutDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}

void CenterOutDisplayPattern::resetInternal(PixelBuffer* pixelBuffer)
{
    m_colorPattern->reset();
    if (m_orientation == CenterOutOrientation::Vertical)
    {
        m_centerLine = pixelBuffer->getRowCount() / 2 - 1;

        int rowsToFill = pixelBuffer->getRowCount() - m_centerLine;
        for (int i = 0; i < rowsToFill; i++)
        {
            updateInternal(pixelBuffer);
        }
    }
    else
    {
        m_centerLine = (pixelBuffer->getColsToLeft() + pixelBuffer->getColsToRight() + pixelBuffer->getColumnCount()) / 2 - 1;

        if (m_centerLine > pixelBuffer->getColsToLeft() + pixelBuffer->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            m_colorPattern->incrementOnly(pixelBuffer->getColsToLeft());
            int colsToFill = pixelBuffer->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelBuffer);
            }
        }
        else if (m_centerLine < pixelBuffer->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            m_colorPattern->incrementOnly(pixelBuffer->getColsToRight());
            int colsToFill = pixelBuffer->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelBuffer);
            }
        }
        else
        {
            // Center line is inside this display.
            int shiftAmount = std::min(pixelBuffer->getColsToLeft(), pixelBuffer->getColsToRight());
            m_colorPattern->incrementOnly(shiftAmount);
            
            int amountToFillLeft = m_centerLine + 1 - pixelBuffer->getColsToLeft();
            int amountToFillRight = pixelBuffer->getColsToLeft() + pixelBuffer->getColumnCount() - (m_centerLine + 1);
            int amountToFill = std::max(amountToFillLeft, amountToFillRight);
            
            for (int i = 0; i < amountToFill; i++)
            {
                updateInternal(pixelBuffer);
            }
        }
    }    
}

void CenterOutDisplayPattern::updateInternal(PixelBuffer* pixelBuffer)
{
    ulong newColor = m_colorPattern->getNextColor();

    if (m_orientation == CenterOutOrientation::Vertical)
    {
        pixelBuffer->shiftRowsUp(newColor, m_centerLine);
        pixelBuffer->shiftRowsDown(newColor, m_centerLine + 1);
    }
    else
    {
        if (m_centerLine > pixelBuffer->getColsToLeft() + pixelBuffer->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            pixelBuffer->shiftColumnsLeft(newColor);
        }
        else if (m_centerLine < pixelBuffer->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            pixelBuffer->shiftColumnsRight(newColor);
        }
        else
        {
            uint localCenter = m_centerLine - pixelBuffer->getColsToLeft();
            pixelBuffer->shiftColumnsLeft(newColor, localCenter);
            if (localCenter < pixelBuffer->getColumnCount() - 1) {
                pixelBuffer->shiftColumnsRight(newColor, localCenter + 1);
            }
        }
    }
}
